#pragma once
# include "Request.hpp"
# include "Response.hpp"
# include <fcntl.h>

#define E500 300
#define E404 204
#define E405 205
#define RESET_ERROR_CODE 200
#define COOKIFILE "/tmp/cookie.txt" 

class Response;
class Request;

class CGI {
    public:
        CGI();
        CGI(int clientSocket, std::vector<ServerConf> &servers);
        ~CGI();
        int CGIHandler(Request &req, Response &resp, int clientSocket);
        int initializeCGIParameters(Request &req, Response &resp);
        int  handlePostMethod(Request &req);
        int executeCGIScript(int clientSocket);
        std::map<std::string, std::string> getCookies() const;
        std::map<std::string, std::string> parseCookies(std::string cookies);
        void redirectURL();
        void checkStatusCode();
        void initHeaders();
        bool isCgiRan() const {return _cgiRan;}
        bool isCgiDone() const {return _isCgiDone;}
        void setisCgiDone(bool isCgiDone) {_isCgiDone = isCgiDone;} 
        std::map<std::string, std::string> getHeaders() const {return _headers;}
        std::string getCgiFd() const {return _cgifd;}
        int getError() const {return _error_code;}
        int getFd() const {return _fd;}
        void findConfig(Request &req);
        void parseHeaders(std::string headers);
    private:
        int _fd;
        std::map<std::string, std::string> _cookies;
        std::string _cgifd;
        std::vector<ServerConf> _servers;
        std::string _filename;
        bool _headersSent;
        std::string _buffer;
        bool _cgiRan;
        bool _isCgiDone;
        std::string _status_code;
        int _error_code;
        Location _location;
        int _clientSocket;
        ServerConf _config;
        std::string _redirect;
        std::string _cgi_path;
        std::string _root;
        std::string _compiler;
        std::map<std::string, std::string> _headers;
        std::vector<std::string> _methods;
        int _pid;
        int _status;
};