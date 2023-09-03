# include "Response.hpp"
# include <iostream>
# include <fcntl.h>
#include <sys/stat.h>
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
    _autoindex = false;
    _isTextStream = false;
    _redirect = "";
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
        case 302:
            _status_code = "302 Moved Permanently";
            break;
        default:
            break;
    }
}

int Response::findRouting(Request &req) {
    std::vector<Location> &locations = _config.location;

    for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        if (req.getPath().find(it->getLocationName()) == 0) {
            std::cout << "Found location" << std::endl;
            _root = it->getString(ROOT);
            _index = it->getString(INDEX);
            _autoindex = it->getAutoindex();
            _errorPages = it->getErrorPages();
            _methods = it->getMethods();
            _redirect = it->getReturned();
            if (_redirect.empty() == false) {
                _error = 302;
                return _error;
            }
            std::string relativePath = req.getPath().substr(it->getLocationName().length());
            if (_autoindex == true) {
                _filePath = constructFilePath(relativePath, _root, _index);
                _isTextStream = false;
                return CONTINUE;
            }
            else {
                _filePath = constructFilePath(relativePath, _root, "");
                _isTextStream = false;
                if (isDirectory(_filePath.c_str())) {
                    _error = 404;
                    return _error;
                }
            }
            return CONTINUE;
        }
    }
    _root = _config.getString(ROOT);
    _index = _config.getString(INDEX);
    _autoindex = _config.getAutoindex();
    _errorPages = _config.getErrorPages();
    _filePath = constructFilePath(req.getPath(), _root, _index);
    if (_autoindex == true) {
        _filePath = constructFilePath(req.getPath(), _root, _index);
        _isTextStream = false;
        return CONTINUE;
    }
    else {
        _filePath = constructFilePath(req.getPath(), _root, "");
        _isTextStream = false;
        if (isDirectory(_filePath.c_str())) {
            _error = 404;
            return _error;
        }
    }
    return CONTINUE;
}

void    Response::handleDefaultError(Request &req) {
    unused(req);
    std::stringstream ss;
    ss << "<center><h1>" << _error << " Error</h1></center>";
    _body = ss.str();
    _fileSize = _body.size();
    _headers["Content-Type"] = "text/html";
    _isTextStream = true;
}

void    Response::handleError(Request &req) {
    if (_error == 302) {
        _fileSize = 0;
        _isTextStream = true;
        return ;
    }
    if (_error == 404) {
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
    }
    
    else if (_error == 501) {
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
    }
    else if (_error == 500) {
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
    }
    else if (_error == 505) {
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
    }
}

void    Response::InitFile(Request &req) {
    int routing = findRouting(req);
    if (routing == 404) {
        handleError(req);
        if (_fd == -1) {
            handleDefaultError(req);
            return ;
        }
        return ;
    }
    else if (routing == 302) {
        handleError(req);
        return ;
    }
    _fd = open(_filePath.c_str(), O_RDONLY);
    if (_fd == -1) {
        _error = 404;
        handleError(req);
        if (_fd == -1) {
           handleDefaultError(req);
           return ;
        }
    }
    _fileSize = lseek(_fd, 0, SEEK_END);
    lseek(_fd, 0, SEEK_SET);
}


void    Response::checkMethod(Request &req) {
    std::string const &method = req.getMethod();
    if (_methods.empty()) {
        _methods.push_back("GET");
        _methods.push_back("POST");
        _methods.push_back("DELETE");
    }
    for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++) {
        if (*it == method)
            return ;
    }
    _error = 501;
    handleError(req);
}

void    Response::checkHttpVersion(Request &req) {
    std::string const &version = req.getHTTPVersion();
    if (version != "HTTP/1.1") {
        _error = 505;
        handleError(req);
    }
}

void Response::InitHeaders(Request &req) {
    std::stringstream ss;
    checkHttpVersion(req);
    checkMethod(req);
    // checkCGI(req);
    findStatusCode(req);
    _headers["Server"] = "Webserv/1.0";
    if (_headers.find("Content-Type") == _headers.end())
        _headers["Content-Type"] = getContentType(_filePath);
    if (!_redirect.empty()) {
        _headers["Location"] = _redirect;
    }
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
        return ERROR;
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
        return ERROR;
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
    if (_isTextStream)
        return sendTextData();
    else 
        return sendFileData();
    
    return CONTINUE;
}
