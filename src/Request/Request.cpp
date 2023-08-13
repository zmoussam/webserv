#include "Request.hpp"

// Parse Content Body
void Request::parseBody(const std::string& request) {
    std::string body;
    std::string::size_type pos = request.find("\r\n\r\n");
    if (pos != std::string::npos) {
        body = request.substr(pos + 4);
    }
    if (body.empty()) {
        return;
    }

    // Remove the "Transfer-Encoding: chunked" footer, if present
    std::string::size_type footerPos = body.find("\r\n0\r\n");
    if (footerPos != std::string::npos) {
        body = body.substr(0, footerPos);
    }

    _body = body;
}

// Parse the "Cookie" header from the request and extract key-value pairs into _cookies map
void Request::parseCookies(const std::string& request) {
	// Extract headers from the request
	std::string headers = getHeaders(request);
	if (headers.empty()) {
		return;
	}

	// Find the "Cookie" header and extract the cookies string
	std::string cookies;
	size_t startPos = headers.find("Cookie: ");
	if (startPos != std::string::npos) {
		startPos += 8; // Move past "Cookie: "
		size_t endPos = headers.find("\r\n", startPos);
		if (endPos != std::string::npos) {
			cookies = headers.substr(startPos, endPos - startPos);
		} else {
			cookies = headers.substr(startPos);
		}
	}

	std::string::size_type pos;
	// Split cookies by "; " delimiter and extract key-value pairs
	while ((pos = cookies.find("; ")) != std::string::npos) {
		std::string cookie = cookies.substr(0, pos);
		cookies.erase(0, pos + 2);
		std::string::size_type equalsPos = cookie.find("=");
		if (equalsPos != std::string::npos) {
			std::string key = cookie.substr(0, equalsPos);
			std::string value = cookie.substr(equalsPos + 1);
			_cookies[key] = value;
		}
	}

	// Handle the last cookie in the string
	std::string::size_type equalsPos = cookies.find("=");
	if (equalsPos != std::string::npos) {
		std::string key = cookies.substr(0, equalsPos);
		std::string value = cookies.substr(equalsPos + 1);
		_cookies[key] = value;
	}
}

// Parse the query parameters from the request URL and extract key-value pairs into _queries map
void Request::parseQueries(const std::string& request) {
	std::string line = getFirstLine(request);
	std::string queries = splitLine(line, 1);
	std::string::size_type pos = queries.find("?");
	if (pos == std::string::npos) {
		return;
	}
	queries.erase(0, pos + 1);
	std::string query;
	std::string key;
	std::string value;

	// Split queries by "&" delimiter and extract key-value pairs
	while ((pos = queries.find("&")) != std::string::npos) {
		query = queries.substr(0, pos);
		queries.erase(0, pos + 1);
		key = query.substr(0, query.find("="));
		value = query.substr(query.find("=") + 1);
		_queries[key] = value;
	}

	// Handle the last query parameter in the string
	key = queries.substr(0, queries.find("="));
	value = queries.substr(queries.find("=") + 1);
	_queries[key] = value;
}

// Parse the headers from the request and extract key-value pairs into _headers map
void Request::parseHeaders(const std::string& request) {
	std::string headers = getHeaders(request);
	if (headers.empty()) {
		return;
	}

	std::string header;
	std::string key;
	std::string value;
	std::string::size_type pos;
	// Split headers by "\r\n" delimiter and extract key-value pairs
	while ((pos = headers.find("\r\n")) != std::string::npos) {
		header = headers.substr(0, pos);
		headers.erase(0, pos + 2);
		key = header.substr(0, header.find(": "));
		value = header.substr(header.find(": ") + 2);
		if (key == "Connection") {
			if (value == "close") {
				_keepAlive = 0;
			}
		}
		if (key == "Cookie") {
			continue;
		}
		_headers[key] = value;
	}
}

// Parse the method, path, and protocol from the first line of the request
void Request::parseMethod(const std::string& request) {
	std::string firstLine = getFirstLine(request);

	_method = splitLine(firstLine, 0);
	_path = splitLine(firstLine, 1);
	_protocol = splitLine(firstLine, 2);
}


int Request::recvRequest(int clientSocket) {
	char buffer[1024];
	int readRes = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (readRes == -1) {
		std::cerr << "Error: recv() failed" << std::endl;
		return DONE;
	}
	else if (readRes == 0) {
		std::cout << "Client disconnected" << std::endl;
		return DISCONNECTED;
	}
	buffer[readRes] = '\0';
	_buffer += buffer;
	if (_buffer.find("\r\n\r\n") != std::string::npos && !_isHeadersRead) {
		_isHeadersRead = 1;
		return DONE;
	}
	return (0);
}

// Handle the request received on the provided client socket
int Request::handleRequest(int clientSocket) {
	// Receive the request from the client
	int rcvRes = recvRequest(clientSocket);
	if (rcvRes == DISCONNECTED) {
		return DISCONNECTED;
	}

		if (rcvRes == DONE && _isHeadersRead) {
			parseMethod(_buffer);
			parseHeaders(_buffer);
			parseQueries(_buffer);
			parseCookies(_buffer);
			// parseBody(_buffer);
		}
	std::cout << " - - " << "\"" << _method << " " << _path << " " << _protocol << "\" "  << std::endl;
	return (0);
}

// Clear the internal data structures of the Request object
void Request::clear(void) {
	_headers.clear();
	_queries.clear();
	_cookies.clear();
}
