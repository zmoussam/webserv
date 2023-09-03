/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmoussam <zmoussam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 18:46:40 by zmoussam          #+#    #+#             */
/*   Updated: 2023/09/03 17:03:20 by zmoussam         ###   ########.fr       */
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
        std::map<std::string , std::string> _cookies;
        bool _keepAlive;
		bool _isHeadersRead;
        int _clientSocket;
        bool _isBodyRead;
        bool _checkBoundary;
    public:
		int recvRequest();
		int handleRequest();
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
        bool KeepAlive() const;
        std::map<std::string, std::string> getHeaders() const;
        std::map<std::string, std::string> getCookies() const;
        void readRequest(int client_socket);
        void parsseRequest();
        void parsseMethod(size_t &methodPos);
        void parssePath_Queries(size_t &URI_Pos);
        void parsseHTTPversion(size_t &httpVersion_pos);
        void parsseHeaders(size_t &header_pos);
        void parsseBody(size_t &bodyPos);
        size_t countboundary(size_t _bodyPos);
        void parsseCookies();
        Request();
        Request(int clientSocket);
        Request(const Request & other);
        Request &operator=(const Request & other);
        ~Request();

};

size_t getBodyLength(std::string Content_length);
int hexStringToInt(const std::string hexString);

#endif