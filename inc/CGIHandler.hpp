#pragma once
#include "Request.hpp"
#include "Response.hpp"

class CGI {
    public:
        CGI();
        CGI(int clientSocket);
        ~CGI();
        int CGIHandler(Request &req, Response &resp, int clientSocket);
        void initializeCGIParameters(Request &req, Response &resp);
        void handlePostMethod(Request &req, std::string &tmpfile);
        void executeCGIScript(int clientSocket);
        void redirectURL();
        void checkStatusCode();
        void initHeaders();
    private:
        std::string _filename;
        bool _headersSent;
        std::string _buffer;
        std::string _status_code;
        int _error_code;
        Location _location;
        int _clientSocket;
		bool _autoindex;
        std::string _redirect;
        std::string _cgi_path;
        std::string _root;
        std::string _compiler;
        std::map<std::string, std::string> _headers;
        std::vector<std::string> _methods;
};