# include "Response.hpp"
# include <iostream>
# include <fcntl.h>

Response::Response()
	: _protocol(""),
	_status_code(""),
	_server("Webserv/1.0"),
	_content_type(""),
	_body(""),
	_headers()
{
	_content_length = 0;
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
	_server("Webserv/1.0"),
	_content_type(""),
	_body(""),
	_headers()
{
	_content_length = 0;
    _dataSent = 0;
    _fd = -1;
    _offset = 0;
    _isCGI = false;
    _filePath = "";
    _buffer = "";
    _clientSocket = clientSocket;
    _headersSent = false;
}

Response::~Response()
{
}

void    Response::InitFile(Request &req) {
    _filePath = constructFilePath(req.getPath());
    _fd = open(_filePath.c_str(), O_RDONLY);


    _fileSize = lseek(_fd, 0, SEEK_END);
    lseek(_fd, 0, SEEK_SET);
    _content_length = _fileSize;
    _content_type = getContentType(_filePath);
}

int Response::sendResp(Request &req) {
    if (_fd == -1) {
        InitFile(req);
    }
    if (_headersSent == false) {
        std::stringstream ss;
        ss << "HTTP/1.1 200 OK\r\n";
        ss << "Server: " << _server << "\r\n";
        ss << "Content-Type: " << _content_type << "\r\n";
        ss << "Content-Length: " << _content_length << "\r\n";
        ss << "\r\n";
        _buffer = ss.str();
        send(_clientSocket, _buffer.c_str(), _buffer.length(), 0);
        _headersSent = true;
    }
    off_t bytesSent = 512;
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