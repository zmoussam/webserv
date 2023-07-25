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

std::string getContentType(std::string filename) {
    std::string type;

	if (filename == "/") {
		filename = "/index.html";
	}
    if (filename.find(".html") != std::string::npos)
        type = "text/html";
    else if (filename.find(".css") != std::string::npos)
        type = "text/css";
    else if (filename.find(".js") != std::string::npos)
        type = "application/javascript";
    else if (filename.find(".jpg") != std::string::npos || filename.find(".jpeg") != std::string::npos)
        type = "image/jpeg";
    else if (filename.find(".png") != std::string::npos)
        type = "image/png";
    else if (filename.find(".gif") != std::string::npos)
        type = "image/gif";
    else if (filename.find(".pdf") != std::string::npos)
        type = "application/pdf";
    // Add more MIME types as needed
    else
        type = "application/octet-stream";

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

void Response::generateResp(Request &request) {
	_protocol = request.getProtocol();
	_status_code = "200 OK";
	_content_type = getContentType(request.getPath());
	generateBody(request);
	_content_length = _body.size();
}

void Response::sendResp(int clientSocket) {
	std::string response = _protocol + " " + _status_code + "\r\n";

	response += "Server: " + _server + "\r\n";
	response += "Content-Type: " + _content_type + "\r\n";
	std::stringstream ss;
	ss << _content_length;
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "\r\n";
	response += _body;
	
	send(clientSocket, response.c_str(), response.length(), 0);
}