# include "Request.hpp"

Request::Request()
	: _method(""),
	  _path(""),
	  _protocol(""),
	  _headers(),
	  _body(""),
	  _queries(),
	  _cookies()
{
}

Request::~Request() {
}

void Request::setMethod(const std::string& method) {
	_method = method;
}

void Request::setPath(const std::string& path) {
	_path = path;
}

void Request::setProtocol(const std::string& protocol) {
	_protocol = protocol;
}

void Request::setBody(const std::string& body) {
	_body = body;
}

void Request::setHeader(const std::string& key, const std::string& value) {
	_headers[key] = value;
}

void Request::setQuery(const std::string& key, const std::string& value) {
	_queries[key] = value;
}

void Request::setCookie(const std::string& key, const std::string& value) {
	_cookies[key] = value;
}


const std::string& Request::getMethod() const {
	return _method;
}

const std::string& Request::getPath() const {
	return _path;
}

const std::string& Request::getProtocol() const {
	return _protocol;
}

const std::string& Request::getBody() const {
	return _body;
}

const std::string& Request::getHeader(const std::string& key) const {
	return _headers.at(key);
}

const std::string& Request::getQuery(const std::string& key) const {
	return _queries.at(key);
}

const std::string& Request::getCookie(const std::string& key) const {
	return _cookies.at(key);
}


// Get the first line of the request
std::string getFirstLine(const std::string& request) {
	std::string firstLine;
	size_t pos = request.find("\r\n");
	if (pos != std::string::npos)
		firstLine = request.substr(0, pos);
	return firstLine;
}

// Split a line by space delimiter and return the token at the specified index
std::string splitLine(const std::string& line, int idx) {
    std::string result;
    std::istringstream iss(line);

    for (int i = 0; i <= idx; i++) {
        std::string token;
        if (std::getline(iss, token, ' ')) {
            result = token;
        } else {
            result.clear();
            break;
        }
    }
    return result;
}

// Get the headers section of the request
std::string getHeaders(const std::string& request) {
	if (request.find("\r\n") == std::string::npos)
		return "";
	std::string headers = request.substr(request.find("\r\n") + 2);
	headers = headers.substr(0, headers.find("\r\n\r\n"));
	return headers;
}
