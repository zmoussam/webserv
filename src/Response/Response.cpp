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
}

Response::~Response()
{
}

std::string getContentType(std::string filename, std::map<std::string, std::string> &mimeTypes) {
    std::string type;

	if (filename == "/") {
		filename = "/index.html";
	}
    std::string extension = filename.substr(filename.find_last_of("."));

    if (mimeTypes.find(extension) != mimeTypes.end()) {
        type = mimeTypes[extension];
    } else {
        type = "text/plain";
    }
    return type;
}

void Response::generateBody(Request &request) {
    std::string path = request.getPath();

    if (path == "/") {
        path = "/index.html";
    }
    path = "www" + path;
    _filePath = path;
    std::ifstream file(path.c_str());
    std::string line;

    if (!file.is_open())
        return;

    // std::stringstream ss;
    // ss << file.rdbuf();
    // _body = ss.str();
    // file.close();

}

void Response::generateResp(Request &request, std::map<std::string, std::string> &mimeTypes) {
	_protocol = request.getProtocol();
	_status_code = "200 OK";
	_content_type = getContentType(request.getPath(), mimeTypes);
	generateBody(request);
    int fd = open(_filePath.c_str(), O_RDONLY);
    // iterate with a buffer of 1024 bytes and append to _body
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, 1024)) > 0) {
        _body.append(buffer, bytesRead);
    }
	_content_length = _body.size();
}

void Response::sendResp(int clientSocket) {
	std::string response = _protocol + " " + _status_code + "\r\n";
	std::stringstream ss;
	response += "Server: " + _server + "\r\n";
	response += "Content-Type: " + _content_type + "\r\n";
	ss << _content_length;
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "\r\n";
	send(clientSocket, response.c_str(), response.length(), 0);
    std::cout << "Sending body\n";
    // off_t toSend = _content_length;
    off_t offset = 0;
    size_t maxSend = _dataSent + offset;
    if (_fd == -1)
        _fd = open(_filePath.c_str(), O_RDONLY);
    if (_fd == -1) {
        perror("Error opening file");
        close(clientSocket);
        return;
    }
    while (_dataSent < _content_length) {
        // use osx sendfile
        sendfile(_fd, clientSocket, _dataSent, &offset, NULL, 0);
        if (offset == 0) {
            std::cout << "offset is 0\n";
            break;
        }
        std::cout << "Sent " << offset << " bytes\n";
        _dataSent += offset;
        maxSend += offset;
        offset = 0;
        
    }
    std::cout << "Sent " << _dataSent << " bytes\n";
    std::cout << "Needs to send " << _content_length << " bytes\n";

    if (_dataSent == _content_length) {
        std::cout << "Closing socket\n";
        close(_fd);
        close(clientSocket);
    }
}