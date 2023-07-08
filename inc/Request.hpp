#pragma once

# include <iostream>
# include <cstring>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <map>
# include <sstream>
#define unused(x) (void)(x)

// Request class is used to store information about the request
// For example, if the client sends a request to the server:

// GET /index.html HTTP/1.1
// Host: example.com
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:85.0) Gecko/20100101 Firefox/85.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
// Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3
// Accept-Encoding: gzip, deflate, br
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// If-Modified-Since: Thu, 11 Feb 2021 16:00:00 GMT

// Then the Request class will store this information:
// _method = GET
// _path = /index.html
// _protocol = HTTP/1.1
// _headers = {
// 	Host: example.com
// 	User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:85.0) Gecko/20100101 Firefox/85.0
// 	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
// 	Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3
// 	Accept-Encoding: gzip, deflate, br
// 	Connection: keep-alive
// 	Upgrade-Insecure-Requests: 1
// 	If-Modified-Since: Thu, 11 Feb 2021 16:00:00 GMT
// }
// _body = ""
// _queries = {}
// _cookies = {}

#define	BUFSIZE 1024

class Request {
	public:
		Request();
		~Request();

		void setMethod(const std::string& method);
		void setPath(const std::string& path);
		void setProtocol(const std::string& protocol);
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& body);
		void setQuery(const std::string& key, const std::string& value);
		void setCookie(const std::string& key, const std::string& value);


		const std::string& getMethod() const;
		const std::string& getPath() const;
		const std::string& getProtocol() const;
		const std::string& getHeader(const std::string& key) const;
		const std::string& getBody() const;
		const std::string& getQuery(const std::string& key) const;
		const std::string& getCookie(const std::string& key) const;

		void handleRequest(int clientSocket);
		void parseMethod(const std::string& request);
		void parseHeaders(const std::string& request);
		void parseQueries(const std::string& request);
		void parseCookies(const std::string& request);
		void clear(void);
	private:
		char _buffer[BUFSIZE];
		std::string _method; // POST, GET, PUT, DELETE, etc.
		std::string _path; // /index.html or /users/1
		std::string _protocol; // HTTP/1.1
		std::map<std::string, std::string> _headers; // Headers are used to send additional information to the server
		std::string _body; // Request body (POST, PUT, etc.) usually JSON format and used to send data to the server
		std::map<std::string, std::string> _queries; // Queries are used to send additional information to the server (GET) https://example.com?query=1&query=2 etc
		std::map<std::string, std::string> _cookies; // Cookies are used to store information about the user (session, etc.)
};


std::string	getFirstLine(const std::string& request);
std::string splitLine(const std::string& line, int idx);
std::string getHeaders(const std::string& request);

