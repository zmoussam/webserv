# include "Response.hpp"
# include <iostream>
# include <fcntl.h>

Response::Response()
	: _protocol(""),
	_status_code(""),
	_body(""),
	_headers()
{
    _dataSent = 0;
    _fd = -1;
    _offset = 0;
    _isCGI = false;
    _filePath = "";
    _buffer = "";
    _clientSocket = -1;
}

Response::Response(int clientSocket)
	: _protocol(""),
	_status_code(""),
	_body(""),
	_headers()
{
    _dataSent = 0;
    _fd = 0;
    _offset = 0;
    _isCGI = false;
    _filePath = "";
    _buffer = "";
    _clientSocket = clientSocket;
    _headersSent = false;
}

Response::Response(int clientSocket, ServerConf &config)
	: _protocol(""),
	_status_code(""),
	_body(""),
	_headers()
{
    _dataSent = 0;
    _fd = 0;
    _offset = 0;
    _isCGI = false;
    _filePath = "";
    _buffer = "";
    _clientSocket = clientSocket;
    _headersSent = false;
    _config = config;
    _error = 0;
}

Response::~Response()
{
}

void    Response::findStatusCode(Request &req) {
    unused(req);
    _status_code = "200 OK";
    switch (_error) {
        case 404:
            _status_code = "404 Not Found";
            break;
        case 500:
            _status_code = "500 Internal Server Error";
            break;
        case 501:
            _status_code = "501 Not Implemented";
            break;
        case 505:
            _status_code = "505 HTTP Version Not Supported";
            break;
        default:
            break;
    }
}

void Response::findRouting(Request &req) {
    std::vector<Location> &locations = _config.location;

    std::cout << "Finding routing" << std::endl;

    _root = _config.getString(ROOT);
    _index = _config.getString(INDEX);
    _autoindex = _config.getAutoindex();
    _filePath = constructFilePath(req.getPath(), _root, _index);
    for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        // Check if request path starts with location name, e.g., /location_name/path/to/file
        // Only check if the location name is at the start of the path
        if (req.getPath().find(it->getLocationName()) == 0) {
            _root = it->getString(ROOT);
            _index = it->getString(INDEX);
            _autoindex = it->getAutoindex();
            _errorPages = it->getErrorPages();
            std::string relativePath = req.getPath().substr(it->getLocationName().length());
            std::cout << "Relative path: " << relativePath << std::endl;
            _filePath = constructFilePath(relativePath, _root, _index);
            std::cout << "Found location" << std::endl;
            std::cout << "Root: " << _root << std::endl;
            std::cout << "Index: " << _index << std::endl;
            std::cout << "Autoindex: " << _autoindex << std::endl;

            return; // Exit the loop if location is found
        }
    }
}

void    Response::handleDefaultError(Request &req) {
    unused(req);
    std::stringstream ss;
    ss << "<center><h1>" << _error << " Error</h1></center>";
    _body = ss.str();
    _fileSize = _body.length();
    _headers["Content-Type"] = "text/html";
    _isTextStream = true;
}


void    Response::InitFile(Request &req) {
    findRouting(req);
    
    std::cout << "File path: " << _filePath << std::endl;
    _fd = open(_filePath.c_str(), O_RDONLY);
    std::cout << "FD: " << _fd << std::endl;
    if (_fd == -1) {
        _error = 404;
        if (_errorPages.empty()) {
            _errorPages = _config.getErrorPages();
            if (_errorPages.empty()) {
                handleDefaultError(req);
                _fileSize = lseek(_fd, 0, SEEK_END);
                lseek(_fd, 0, SEEK_SET);
                return ;
            }
        }
        _filePath = constructFilePath(_errorPages[_error], _root, _index);
        std::cout << "Error file path: " << _filePath << std::endl;
        _fd = open(_filePath.c_str(), O_RDONLY);
        if (_fd == -1) {
           handleDefaultError(req);
           return ;
        }
    }
    _fileSize = lseek(_fd, 0, SEEK_END);
    lseek(_fd, 0, SEEK_SET);
}


void Response::InitHeaders(Request &req) {
    std::stringstream ss;

    findStatusCode(req);
    _headers["Server"] = "Webserv/1.0";
    if (_headers.find("Content-Type") == _headers.end())
        _headers["Content-Type"] = getContentType(_filePath);
    ss << _fileSize;
    _headers["Content-Length"] = ss.str();
    _headers["Connection"] = "keep-alive";
}



# ifdef __APPLE__
int Response::sendFileData() {
    off_t bytesSent = 1024;
    int res = sendfile(_fd, _clientSocket, _offset, &bytesSent, NULL, 0);
    if (res == -1 && _offset >= _fileSize) {
        return DONE;
    }
    _offset += bytesSent;
    if (_offset >= _fileSize) {
        close(_fd);
        _fd = -1;
        _offset = 0;
        return DONE;
    }
    return CONTINUE;
}
# else
#include <sys/sendfile.h>

int Response::sendFileData() {
    off_t offset = _offset; // Save the offset before modifying it
    off_t remainingBytes = _fileSize - offset;
    
    ssize_t bytesSent = sendfile(_clientSocket, _fd, &offset, remainingBytes);
    if (bytesSent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Handle the case where sendfile would block (non-blocking socket)
            return CONTINUE;
        } else {
            // Handle other errors
            return ERROR;
        }
    } else if (bytesSent == 0 && offset >= _fileSize) {
        close(_fd);
        _fd = -1;
        _offset = 0;
        return DONE;
    }

    _offset = offset;
    if (_offset >= _fileSize) {
        close(_fd);
        _fd = -1;
        _offset = 0;
        return DONE;
    }
    return CONTINUE;
}
#endif

int Response::sendTextData() {
    std::string &body = _body;
    size_t remainingBytes = body.length() - _dataSent;
    ssize_t bytesSent = send(_clientSocket, body.c_str() + _dataSent, remainingBytes, 0);
    _dataSent += bytesSent;
    if (bytesSent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Handle the case where sendfile would block (non-blocking socket)
            return CONTINUE;
        } else {
            // Handle other errors
            return ERROR;
        }
    } else if (bytesSent == 0 && _dataSent >= body.length()) {
        _dataSent = 0;
        return DONE;
    }

    _dataSent += bytesSent;
    if (_dataSent >= body.length()) {
        _dataSent = 0;
        return DONE;
    }
    return CONTINUE;
}

int Response::sendResp(Request &req) {
    std::stringstream ss;
    if (_fd == 0) {
        InitFile(req);
        InitHeaders(req);
    }
    if (_headersSent == false) {
        ss << "HTTP/1.1 " << _status_code << "\r\n";
        for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++) {
            ss << it->first << ": " << it->second << "\r\n";
        }
        ss << "\r\n";
        _buffer = ss.str();
        send(_clientSocket, _buffer.c_str(), _buffer.length(), 0);
        _headersSent = true;
    }
    if (!_isTextStream)
        return sendFileData();
    else 
        return sendTextData();
    
    return CONTINUE;
}
