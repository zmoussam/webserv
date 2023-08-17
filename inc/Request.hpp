/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmoussam <zmoussam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 18:46:40 by zmoussam          #+#    #+#             */
/*   Updated: 2023/08/17 20:09:14 by zmoussam         ###   ########.fr       */
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
struct BoundaryBody
{
    std::map<std::string, std::string> headers;
    std::vector<std::string> _body;
    std::string _boundary;
    size_t _bodysCount;
    BoundaryBody();
} ;

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
        BoundaryBody _boundaryBody;
        std::map<std::string, std::string> _headers;
        std::map<std::string , std::string> _cookies;
        bool _keepAlive;
		bool _isHeadersRead;
        int _clientSocket;
        bool _isBodyRead;
        bool _checkBoundary;
    public:
        // std::string _requestHeaders;
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
        void parsseCookies();
        Request();
        Request(int clientSocket);
        ~Request();
        Request & operator=(const Request & other) {
            if (this != &other) {
                this->_REQ.str(other._REQ.str());
                this->_request = other._request;
                this->_requestLength = other._requestLength;
                this->_httpVersion = other._httpVersion;
                this->_body = other._body;
                this->_URI = other._URI;
                this->_method = other._method;
                this->_queries = other._queries;
                this->_headers = other._headers;
                this->_cookies = other._cookies;
                this->_keepAlive = other._keepAlive;
                this->_isHeadersRead = other._isHeadersRead;
                this->_clientSocket = other._clientSocket;
                this->_isBodyRead = other._isBodyRead;
                this->_checkBoundary = other._checkBoundary;
            }
            return *this;
        }
};

size_t getBodyLength(std::string Content_length);
int hexStringToInt(const std::string hexString);

#endif