#include "Request.hpp"

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

// Handle the request received on the provided client socket
void Request::handleRequest(int clientSocket) {
	// Receive the request from the client
	ssize_t bytesRead = recv(clientSocket, _buffer, BUFSIZE, 0);
	std::string request;

	if (bytesRead < 0) {
		std::cerr << "Error: recv() failed" << std::endl;
		_exit(1);
	}

	_buffer[bytesRead] = '\0';
	request = _buffer;

	// Parse the request components
	parseMethod(request);
	parseHeaders(request);
	parseQueries(request);
	parseCookies(request);


	// "log the request"
	std::cout << " - - " << "\"" << _method << " " << _path << " " << _protocol << "\" "  << std::endl;
	
	// !! TESTING CODE !! //
	// std::cout << request << std::endl;
	// std::cout << "Method: " << _method << std::endl;
	// std::cout << "Path: " << _path << std::endl;
	// std::cout << "Arguments: " << std::endl;
	// std::map<std::string, std::string>::iterator it = _queries.begin();
	// while (it != _queries.end()) {
	// 	std::cout << it->first << ":= " << it->second << std::endl;
	// 	it++;
	// }
	// std::cout << "Cookies: " << std::endl;
	// std::map<std::string, std::string>::iterator it2 = _cookies.begin();
	// while (it2 != _cookies.end()) {
	// 	std::cout << it2->first << ":= " << it2->second << std::endl;
	// 	it2++;
	// }
	// std::cout << "Headers: " << std::endl;
	// std::map<std::string, std::string>::iterator it3 = _headers.begin();
	// while (it3 != _headers.end()) {
	// 	std::cout << it3->first << ":= " << it3->second << std::endl;
	// 	it3++;
	// }

	// Send a response back to the client (for testing purposes for now	)
	// std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nServer: Webserv\r\n\r\n<center><h1>OK</h1></center>";
	// ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
	// if (bytesSent < 0) {
	// 	std::cerr << "Error: send() Request.cpp 83:0" << std::endl;
	// }
	// !! TESTING CODE !! //
}

// Clear the internal data structures of the Request object
void Request::clear(void) {
	_headers.clear();
	_queries.clear();
	_cookies.clear();
}
