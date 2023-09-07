// Needs to be implemented when Server is done
#pragma once
#include "Request.hpp"
#include "Response.hpp"

class CGI {
    public:
        CGI();
        CGI(int clientSocket);
        ~CGI();
        int CGIHandler(Request &req, Response &resp, int clientSocket);
    private:
        Location _location;
        int _clientSocket;
		bool _autoindex;
        std::string _redirect;
        std::string _cgi_path;
        std::string _root;
        std::string _compiler;
        std::vector<std::string> _methods;
};