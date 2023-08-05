# include "Response.hpp"

Response::Response()
	: _protocol(""),
	_status_code(""),
	_server("Webserv/1.0"),
	_content_type(""),
	_body(""),
	_headers()
{
	_content_length = 0;
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

    std::ifstream file(path.c_str());
    std::string line;

    if (!file.is_open())
        return;

    while (std::getline(file, line)) {
        _body += line;
        _body += "\n";
    }
}

void Response::generateResp(Request &request, std::map<std::string, std::string> &mimeTypes) {
	_protocol = request.getProtocol();
	_status_code = "200 OK";
	_content_type = getContentType(request.getPath(), mimeTypes);
	generateBody(request);
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
	response += _body;
	
	send(clientSocket, response.c_str(), response.length(), 0);
}