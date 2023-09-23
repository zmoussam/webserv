/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmoussam <zmoussam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 18:46:40 by zmoussam          #+#    #+#             */
/*   Updated: 2023/09/23 16:59:28 by zmoussam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  REQUEST__
#define REQUEST__
#include <iostream>
#include <string>
#include <map>
#include <sys/socket.h>
#include <sstream>
#include <list>
#include <vector>
#include "Macros.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include <cstdlib>
struct BoundaryBody
{
    std::map<std::string, std::string> headers;
    std::string filename;
    bool _isFile;
    std::string _body;
    struct BoundaryBody *next;
};

class Request
{
    private:
        std::stringstream _REQ;
        std::string _request;
        size_t _requestLength;
        std::string _httpVersion;
        std::string _body;
        std::string _URI;
        std::string _method;
        std::string _queries;
        std::string _boundary;
        BoundaryBody *_boundaryBody;
        std::map<std::string, std::string> _headers;
        std::string _cookies;
        ServerConf _config;
        std::vector<ServerConf> _servers;
        size_t _bodySize;
        int _error;
        bool _keepAlive;
		bool _isHeadersRead;
        int _clientSocket;
        bool _isBodyRead;
        bool _checkBoundary;
        int _port;
    public:
		int recvRequest();
		int handleRequest(int port);
		bool isHeadersRead() const;
		bool isBodyRead() const;
        int waitForBody(size_t headerLength);
        std::string getFullRequest() const;
        size_t getRequestLength() const;
        std::string getBody() const;
        std::string getHTTPVersion() const;
        std::string getPath() const;
        std::string getMethod() const;
        std::string getQueries() const;
        std::string getCookies() const {return _cookies;}
        BoundaryBody *getBoundaryBody() const {return _boundaryBody;}
        int     getPort() const {return _port;}
        bool KeepAlive() const;
        std::map<std::string, std::string> getHeaders() const;
        // std::map<std::string, std::string> getCookies() const;
        int getError() const {return _error;}
        void readRequest(int client_socket);
        void parsseRequest();
        void parsseMethod(size_t &methodPos);
        void parssePath_Queries(size_t &URI_Pos);
        void parsseHTTPversion(size_t &httpVersion_pos);
        void parsseHeaders(size_t &header_pos);
        void parsseBody(size_t &bodyPos);
        void getChunkedBody(size_t &_bodyPos);
        void getBoundaries(size_t &_bodyPos);
        size_t countboundaries(size_t _bodyPos);
        void findConfig();
        void creatUploadFile(BoundaryBody *headBoundaryBody);
        std::string generateRandomString();
        std::string getUploadPath();
        void creatFile(std::string fileName, std::string body);
        void parsseCookies();
        Request();
        Request(int clientSocket, std::vector<ServerConf> servers);
        Request(const Request & other);
        Request &operator=(const Request & other);
        ~Request();

};

size_t getBodyLength(std::string Content_length);
int hexStringToInt(const std::string hexString);

#endif