#include "Response.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
Response::Response() :
      _protocol(""),
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
    _clientSocket = -1;
    _headersSent = false;
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

Response::Response(int clientSocket, std::vector<ServerConf> &servers)
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
    _servers = servers;
    _error = 0;
    _autoindex = false;
    _isTextStream = false;
    _redirect = "";
}

Response::~Response()
{
}

std::string Response::findStatusCode(int code)
{
    std::string status_code;
    switch (code)
    {
        case 0:
            status_code = "200 OK";
            break;
        case 201:
            status_code = "201 Created";
            break;
        case 400:
            status_code = "400 Bad Request";
            break;
        case 301:
            status_code = "301 Moved Permanently";
            break;
        case 302:
            status_code = "302 Moved Temporarily";
            break;
        case 404:
            status_code = "404 Not Found";
            break;
        case 405:
            status_code = "405 Method Not Allowed";
            break;
        case 413:
            status_code = "413 Payload Too Large";
            break;
        case 500:
            status_code = "500 Internal Server Error";
            break;
        case 501:
            status_code = "501 Not Implemented";
            break;
        case 505:
            status_code = "505 HTTP Version Not Supported";
            break;
        default:
            status_code = "200 OK";
            break;
    }
    return status_code;
}

/**
 * @brief Finds the appropriate location block for the given request and sets the corresponding response properties.
 * 
 * @param req The request object.
 * @return int Returns CONTINUE if the request can be processed, otherwise returns an error code.
 */
int Response::findRouting(Request &req)
{
    std::vector<Location> &locations = _config.location;

    for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if (req.getPath().find(it->getLocationName()) == 0)
        {
            _location = *it;
            _root = it->getString(ROOT);
            _index = it->getString(INDEX);
            _autoindex = it->getAutoindex();
            _errorPages = it->getErrorPages();
            _methods = it->getMethods();
            _redirect = it->getReturned();

            int idx = req.getPath().find(it->getLocationName());
            std::string locationName = it->getLocationName();
            std::string relativePath = (req.getPath().find_first_of(locationName) == 0) ?
                                      req.getPath().substr(locationName.length()) :
                                      req.getPath().substr(0, idx);

            _filePath = constructFilePath(relativePath, _root, "");

            if (isDirectory(_filePath.c_str()))
                _filePath = constructFilePath(relativePath, _root, _index);

            if (_filePath[_filePath.length() - 1] == '/')
                _filePath = _filePath.substr(0, _filePath.length() - 1);

            if (!_redirect.empty() && (req.getPath().substr(it->getLocationName().length()) == "/" ||
                                       req.getPath().substr(it->getLocationName().length()) == "") &&
                req.getMethod() == "GET")
            {
                _error = 301;
                return _error;
            }

            if (_autoindex)
            {
                _filePath = constructFilePath(relativePath, _root, "");
                _isTextStream = true;
                return CONTINUE;
            }

            return CONTINUE;
        }
    }

    _root = _config.getString(ROOT);
    _index = _config.getString(INDEX);
    _autoindex = _config.getAutoindex();
    _errorPages = _config.getErrorPages();
    _methods = _config.getMethods();
    _filePath = constructFilePath(req.getPath(), _root, _index);

    if (_autoindex)
    {
        _filePath = constructFilePath(req.getPath(), _root, "");
        _isTextStream = true;
        return CONTINUE;
    }
    else
    {
        _filePath = constructFilePath(req.getPath(), _root, _index);
        _isTextStream = false;
    }

    return CONTINUE;
}


/**
 * @brief Handles default error by setting the status code, body, headers and initializing headers.
 * 
 * @param code The error code.
 */
void Response::handleDefaultError(int code)
{
    std::stringstream ss;

    _status_code = findStatusCode(code);
    ss << "<center><h1>" << _status_code << "</h1></center>";
    _body = ss.str();
    _fileSize = _body.size();
    _headers["Content-Type"] = "text/html";
    _isTextStream = true;
    InitHeaders(_request);
}

void Response::handleError(int code)
{
    if (code == 301)
    {
        _status_code = findStatusCode(code);
        _fileSize = 0;
        _isTextStream = true;
        InitHeaders(_request);
    }
    else
    {
        _status_code = findStatusCode(code);
        std::string errorPage = "";
        if (_errorPages.empty())
        {
            if (_location.getErrorPages().empty())
            {
               if (_config.getErrorPages().empty())
               {
                   handleDefaultError(code);
                   return;
               }
               else
               {
                   _errorPages = _config.getErrorPages();
               }
            }
            else
            {
                _errorPages = _location.getErrorPages();
            }
        }
        if (_errorPages.find(code)->second.find("/") == 0)
            errorPage = _errorPages.find(code)->second;
        else {
            errorPage = constructFilePath(_errorPages.find(code)->second, _root, "");
        }
        if (isDirectory(errorPage.c_str())) {
            handleDefaultError(code);
            return;
        }
        _fd = open(errorPage.c_str(), O_RDONLY);
        if (_fd == -1)
        {
            handleDefaultError(code);
            return;
        }
        _fileSize = lseek(_fd, 0, SEEK_END);
        lseek(_fd, 0, SEEK_SET);
        _isTextStream = false;
    }
    InitHeaders(_request);
}

std::string findDirname(const std::string& path, const std::string& root)
{
    // Find the position where root ends in the path
    size_t rootPos = path.find(root);
    if (rootPos == std::string::npos)
        return "";

    // Remove root from the path
    std::string dirname = path.substr(rootPos + root.length());

    // Find the last '/' character in the remaining path
    size_t pos = dirname.find_last_of('/');
    if (pos == std::string::npos)
        return "";

    // Extract the dirname
    dirname = dirname.substr(0, pos);

    return dirname;
}


void Response::genListing()
{
    std::string path = _filePath;
    DIR *dir = opendir(path.c_str());

    if (dir == NULL)
    {
        _error = ISFILE;
        return;
    }

    std::stringstream ss;
    std::string location = _location.getLocationName();
    std::string pathName = location.empty() ? findDirname(path, _root) + "/" : location + findDirname(path, _root) + "/";

    ss << "<html><head><title>Index of " << pathName << "</title></head><body bgcolor=\"white\"><h1>Index of " << pathName << "</h1><hr><pre>";

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        ss << "<a href=\"" << pathName << ent->d_name << "\">" << ent->d_name << "</a><br>";
    }

    closedir(dir);

    ss << "</pre><hr></body></html>";
    _body = ss.str();
    _fileSize = _body.size();
    _isTextStream = true;
    InitHeaders(_request);
    
}


void Response::InitFile(Request &req) {
    int routing = findRouting(req);
    if (checkMethod(req) == ERROR)
        throw HTTPException("Method not allowed", 405);
    if (checkHttpVersion(req) == ERROR)
        throw HTTPException("HTTP version not supported", 505);
    _error = req.getError();
    if (_error != 0) {
        throw HTTPException("Request error", _error);
    }
    if (routing == 301) {
        throw HTTPException("Moved Temporarily", 302);
    }

    if (routing == 404 || (isDirectory(_filePath.c_str()) && _autoindex == false)) {
        _error = 404;
        throw HTTPException("Not Found", 404);
    }


    if (_autoindex && _isTextStream) {
        genListing();
        if (_error != ISFILE) {
            
            return;
        }
        _isTextStream = false;
        _error = 0;
    }
    _fd = open(_filePath.c_str(), O_RDONLY);
    if (_fd == -1 && _error != ISFILE)
        throw HTTPException("Not Found", 404);
    _fileSize = lseek(_fd, 0, SEEK_END);
    lseek(_fd, 0, SEEK_SET);
    InitHeaders(req);
}


int Response::checkMethod(Request &req)
{
    std::string const &method = req.getMethod();
    if (_methods.empty())
    {
        _methods.push_back("GET");
        _methods.push_back("POST");
        _methods.push_back("DELETE");
    }
    for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++)
    {
        if (*it == method)
            return CONTINUE;
    }
    _error = 405;
    throw HTTPException("Method not allowed", 405);
    return ERROR;
}

int Response::checkHttpVersion(Request &req)
{
    std::string const &version = req.getHTTPVersion();
    if (version != "HTTP/1.1")
    {
        _error = 505;
        throw HTTPException("HTTP version not supported", 505);
        return ERROR;
    }
    return CONTINUE;
}

void Response::InitHeaders(Request &req)
{
    std::stringstream ss;
    
    _headers["Server"] = "Webserv/1.0";

    // Check if Content-Type is not set
    if (_headers.find("Content-Type") == _headers.end())
    {
        _headers["Content-Type"] = getContentType(_filePath);
    }

    // Check _isTextStream and _redirect
    if (_isTextStream == true && _redirect.empty())
    {
        ss << _body.length();
        _headers["Content-Length"] = ss.str();
        _headers["Connection"] = req.isKeepAlive() ? "keep-alive" : "close";
        return;
    }

    // Check _redirect
    if (!_redirect.empty())
    {
        _headers["Location"] = _redirect;
        _error = 301;
    }

    // Set Content-Length and Connection
    ss << _fileSize;
    _headers["Content-Length"] = ss.str();
    _headers["Connection"] = req.isKeepAlive() ? "keep-alive" : "close";
}

#ifdef __APPLE__
int Response::sendFileData()
{
    off_t bytesSent = 1024;
    int res = sendfile(_fd, _clientSocket, _offset, &bytesSent, NULL, 0);
    if (res == -1 && _offset >= _fileSize)
    {
        close(_fd);
        return DONE;
    }
    _offset += bytesSent;
    if (_offset >= _fileSize)
    {
        if (_fd > 0)
        {
        close(_fd);
        return DONE;
        }
    }
    return CONTINUE;
}
#else
#include <sys/sendfile.h>

int Response::sendFileData()
{
    off_t offset = _offset; // Save the offset before modifying it
    off_t remainingBytes = _fileSize - offset;

    ssize_t bytesSent = sendfile(_clientSocket, _fd, &offset, remainingBytes);
    if (bytesSent == -1)
    {
        return ERROR;
    }
    else if (bytesSent == 0 && offset >= _fileSize)
    {
        close(_fd);
        _fd = -1;
        _offset = 0;
        return DONE;
    }

    _offset = offset;
    if (_offset >= _fileSize)
    {
        close(_fd);
        _fd = -1;
        _offset = 0;
        return DONE;
    }
    return CONTINUE;
}
#endif

int Response::sendTextData()
{
    std::string &body = _body;
    ssize_t remainingBytes = body.length() - _dataSent;
    ssize_t bytesSent = send(_clientSocket, body.c_str() + _dataSent, remainingBytes, 0);
    _dataSent += bytesSent;
    if (bytesSent == -1)
    {
        return ERROR;
    }
    else if (bytesSent == 0 && _dataSent >= body.length())
    {
        _dataSent = 0;
        return DONE;
    }
    _dataSent += bytesSent;
    if (_dataSent >= body.length())
    {
        _dataSent = 0;
        return DONE;
    }
    return CONTINUE;
}

void    Response::findConfig(Request &req)
{
    const std::map<std::string, std::string> &headers = req.getHeaders();
    if (headers.find("Host") != headers.end())
    {
        std::string host = headers.find("Host")->second;
        int port = req.getPort();
        for (std::vector<ServerConf>::iterator it = _servers.begin(); it != _servers.end(); it++)
        {
            if (it->getString(SERVER_NAME) == host || it->getString(SERVER_NAME) + ":" + std::to_string(port) == host)
            {
                _config = *it;
                return;
            }
        }
    }
    _config = _servers[0];
}


void    Response::handleCGI() {
    if (_cgi && _cgi->isCgiDone() == true)
    {
        _fd = _cgi->getFd();
        _error = _cgi->getError();
        if (_error != 0)
            throw HTTPException("CGI error", _error);
        else {
            _isCGI = true;
            _fileSize = lseek(_fd, 0, SEEK_END);
            lseek(_fd, 0, SEEK_SET);
            _headers = _cgi->getHeaders();
            _headers["Content-Length"] = std::to_string(_fileSize);
            std::map<std::string, std::string> cookies = _cgi->getCookies();
            for (std::map<std::string, std::string>::iterator it = cookies.begin(); it != cookies.end(); it++)
                _headers["Set-Cookie"] = it->first + "=" + it->second;
            
        }
    }
}

int Response::sendResp(Request &req, CGI *cgi)
{
    try {
        findConfig(req);
        _cgi = cgi;
        handleCGI();
        if (_fd == 0 && _isCGI == false) {
            InitFile(req);
        }
    }
    catch (HTTPException &e) {
        _error = e.getErrorCode();
        handleError(e.getErrorCode());
    
    }
    catch (std::exception &e) {
        _error = 500;
        handleError(_error);
    
    }
    catch (...) {
        _error = 500;
        handleError(_error);
    }
    std::stringstream ss;
    if (_headersSent == false)
    {
        if (_error == 0)
            _status_code = findStatusCode(0);
        ss << "HTTP/1.1 " << _status_code << "\r\n";
        for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
        {
            ss << it->first << ": " << it->second << "\r\n";
        }
        ss << "\r\n";
        _buffer = ss.str();
        send(_clientSocket, _buffer.c_str(), _buffer.length(), 0);
        _headersSent = true;
    }
    if (_redirect.empty() == false)
        return DONE;
    if (_isTextStream)
        return sendTextData();
    else
        return sendFileData();
    return CONTINUE;

}