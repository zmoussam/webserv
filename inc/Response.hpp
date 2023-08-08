#pragma once

# include <string>
# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fstream>
# include <sstream>

# include "Request.hpp"
#include "Macros.hpp"

class Response
{
	public:
		Response();
		~Response();
		void	generateResp(Request &request, std::map<std::string, std::string> &mimeTypes);
		void	generateBody(Request &request);
		void	sendResp(int clientSocket);

		void 	setSocket(int clientSocket) { _clientSocket = clientSocket; }
		int 	getSocket() const { return _clientSocket; }
	private:
		int _clientSocket;
		std::string _protocol;
		std::string _status_code;
		std::string _server;
		std::string _content_type;
		u_int64_t _content_length;
		std::string _body;
		std::map<std::string, std::string> _headers;
		std::string _filePath;
};

std::string getContentType(std::string filename);
