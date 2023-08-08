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
		std::string getBody() const { return _body; }
		size_t getDataRemaining() const { return _dataRemaining; }
		size_t getDataSent() const { return _dataSent; }
		void updateDataSent(size_t dataSent) { _dataSent += dataSent; }
		std::string getFilePath() const { return _filePath; }
	private:
		int _clientSocket;
		std::string _protocol;
		std::string _status_code;
		std::string _server;
		std::string _content_type;
		u_int64_t _content_length;
		std::string _body;
		std::map<std::string, std::string> _headers;
		size_t _dataSent;
		size_t _dataRemaining;
		std::string _filePath;
		int _fd;
};

std::string getContentType(std::string filename);
