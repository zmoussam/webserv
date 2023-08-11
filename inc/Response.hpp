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
		Response(int clientSocket);

		~Response();
		void    InitFile(Request &req);
		int	sendResp(Request &req);

		void 	setSocket(int clientSocket) { _clientSocket = clientSocket; }
		int 	getSocket() const { return _clientSocket; }
		std::string getBody() const { return _body; }
		size_t getDataSent() const { return _dataSent; }
		void updateDataSent(size_t dataSent) { _dataSent += dataSent; }
		std::string getFilePath() const { return _filePath; }
	private:
		int _clientSocket;
		int _fd;
		bool _isCGI;
		off_t _offset;
		size_t _dataSent;
		off_t _fileSize;
		bool _headersSent;
		std::string _protocol;
		std::string _status_code;
		std::string _server;
		std::string _content_type;
		u_int64_t _content_length;
		std::string _body;
		std::map<std::string, std::string> _headers;
		std::string _filePath;
		std::string _buffer;
};

std::string getContentType(std::string filename);
