#pragma once

# include <string>
# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fstream>
# include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
# include "Config.hpp"
# include "Request.hpp"
#include "Macros.hpp"

# define ERROR_404 "<h1>404 Not Found</h1>"

class Response
{
	public:
		Response();
		Response(int clientSocket);
		Response(int clientSocket, ServerConf &config);

		~Response();
		void    	InitFile(Request &req);
		void    	InitHeaders(Request &req);
		std::string checkFilePath(Request &req);
		void		handleDefaultError(Request &req);
		int		sendError(std::string &root, std::map<int, std::string> &errpages);
		int		sendResp(Request &req);
		int	findRouting(Request &req);
		void	findStatusCode(Request &req);
		void 	setSocket(int clientSocket) { _clientSocket = clientSocket; }
		int 	getSocket() const { return _clientSocket; }
		std::string getBody() const { return _body; }
		size_t getDataSent() const { return _dataSent; }
		void updateDataSent(size_t dataSent) { _dataSent += dataSent; }
		std::string getFilePath() const { return _filePath; }
		int		sendFileData();
		int 	sendTextData();
		void	checkMethod(Request &req);
		void	checkHttpVersion(Request &req);
		void	handleError(Request &req);
	private:
		ServerConf _config;
		Location _location;
		Request _request;
		int _clientSocket;
		int _fd;
		bool _isCGI;
		off_t _offset;
		size_t _dataSent;
		off_t _fileSize;
		bool _headersSent;
		std::string _protocol;
		std::string _status_code;
		std::string _body;
		std::map<std::string, std::string> _headers;
		std::string _filePath;
		std::string _buffer;
		int _error;
		std::string _root;
		std::string _index;
		std::vector<std::string> _methods;
		std::string _returnPath;
		bool _autoindex;
		std::map <int, std::string> _errorPages;
		bool	_isTextStream;
		std::string _redirect;
};

std::string getContentType(std::string filename);
std::string constructFilePath(const std::string& requestPath, const std::string &root, const std::string &index);
bool isDirectory(const char* path);