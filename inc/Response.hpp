#pragma once

# include <string>
# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fstream>
# include <sstream>
# include "Request.hpp"

class Response
{
	public:
		Response();
		~Response();
		void	generateResp(Request &request);
		void	generateBody(Request &request);
		void	sendResp(int clientSocket);
	private:
		std::string _protocol;
		std::string _status_code;
		std::string _server;
		std::string _content_type;
		u_int64_t _content_length;
		std::string _body;
		std::map<std::string, std::string> _headers;
};

std::string getContentType(std::string filename);