/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmoussam <zmoussam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 21:46:08 by zmoussam          #+#    #+#             */
/*   Updated: 2023/08/13 18:55:05 by zmoussam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Macros.hpp"
#include <sys/socket.h>
#include <sstream>

int Request::recvRequest() {
	char buffer[1024];
	int readRes = recv(_clientSocket, buffer, sizeof(buffer), 0);
	if (readRes == -1) {
		std::cerr << "Error: recv() failed" << std::endl;
		return DONE;
	}
	else if (readRes == 0) {
		std::cout << "Client disconnected" << std::endl;
		return DISCONNECTED;
	}
	buffer[readRes] = '\0';
	_request += buffer;
	if (_request.find("\r\n\r\n") != std::string::npos && !_isHeadersRead) {
		_isHeadersRead = 1;
		return DONE;
	}
	return (0);
}

// Handle the request received on the provided client socket
int Request::handleRequest() {
	// Receive the request from the client
	int rcvRes = recvRequest();
	if (rcvRes == DISCONNECTED) {
		return DISCONNECTED;
	}

		if (rcvRes == DONE && _isHeadersRead) {
			parsseRequest();
		}
	// std::cout << " - - " << "\"" << _method << " " << _path << " " << _protocol << "\" "  << std::endl;
	return (0);
}

Request::Request(int clientSocket) 	: 
	_request(""),
    _requestLength(0),
    _httpVersion(""),
	_body(""),
	_URI(""),
    _method(""),
	_queries(),
	_headers(),
	_cookies(),
	_keepAlive(1),
	_isHeadersRead(false)
{
    readRequest(clientSocket); 
}

// void Request::readRequest(int client_socket)
// {
//     char buffer[11];
//     int rd = 1;
    
//     while (rd != 0)
//     {
//         memset(buffer, 0, 11);
//         rd = recv(client_socket, buffer, 10, 0);
//         if (rd < 0)
//         {
//             std::cout << "Error : recieve request failed \n" << strerror(errno) << std::endl;
//             return;
//         }
//         buffer[rd] = '\0';
//         _request += std::string(buffer);
//         if (_request.find("\r\n\r\n") != std::string::npos)
//             break;
//     }
    
//     _requestLength = _request.size();
//     // std::cout << _request << std::endl;
// }

void Request::parsseRequest()
{
    size_t nextPos = 0;
    parsseMethod(nextPos);
    parssePath_Queries(nextPos);
    parsseHTTPversion(nextPos);
    parsseHeaders(nextPos);
    parsseBody(nextPos);
    parsseCookies();

}

void Request::parsseMethod(size_t &methodPos)
{
    for (; methodPos < _requestLength && _request[methodPos] != ' '; methodPos++)
        _method += _request[methodPos];
}

void Request::parssePath_Queries(size_t &URI_Pos)
{
    std::string URI = "";
    size_t queryPos;
    for (; URI_Pos < _requestLength && _request[URI_Pos] == ' '; URI_Pos++);
    for (; URI_Pos < _requestLength && _request[URI_Pos] != ' '; URI_Pos++)
        URI += _request[URI_Pos];
    queryPos = URI.find('?');
    if (queryPos != std::string::npos)
    {
        _URI = URI.substr(0, queryPos);
        _queries = URI.substr(queryPos + 1);
    }
    else {
        _URI = URI;
    }
}

void Request::parsseHTTPversion(size_t &_httpversion_Pos)
{
    for (; _httpversion_Pos < _requestLength && _request[_httpversion_Pos] == ' '; _httpversion_Pos++);
    for (; _httpversion_Pos < _requestLength && _request[_httpversion_Pos] != ' ' \
       && _request[_httpversion_Pos] != '\r'; _httpversion_Pos++)
        _httpVersion += _request[_httpversion_Pos];
}

void Request::parsseHeaders(size_t &_hpos)
{
    std::string _key;
    size_t _headerkeyPos;
    size_t _headerValuePos;
    _keepAlive = false;
    size_t bodypos = _request.find("\r\n\r\n", _hpos);
    if (bodypos == std::string::npos)
        return;
    while (_hpos < bodypos)
    {
        _headerkeyPos = _request.find(':', _hpos);
        if (_headerkeyPos == std::string::npos)
            break;
        _key = _request.substr(_hpos + 2, _headerkeyPos - _hpos - 2);
        if ((_headerValuePos = _request.find("\r\n", _headerkeyPos)) == std::string::npos)
            return;
        _headers[_key] = _request.substr(_headerkeyPos + 2, _headerValuePos - _headerkeyPos - 2);
        if (_key == "Connection" && _headers[_key] == "keep-alive")
            _keepAlive = true;
        _hpos = _headerValuePos;
        // std::cout << '$' << _key << "$ : " << '$' << this->_headers[_key]  << "$" << std::endl;
    }
}

void Request::parsseCookies()
{
    std::string _key;
    std::string _value;
    std::string Cookies;
    if (_headers.find("Cookie") != _headers.end() || _headers.find("Cookies") != _headers.end()) {
        if (_headers.find("Cookie") != _headers.end()) { Cookies = _headers["Cookie"]; }
        else { Cookies = _headers["Cookies"]; }
        size_t CookiesLength = Cookies.size();
        for (size_t i = 0; i < CookiesLength; i++) {
            _key = "" , _value = "";
            for (; i < CookiesLength && Cookies[i] != '='; i++)
                if (Cookies[i] != ' ')
                    _key +=  Cookies[i];
            i++;
            for (; i < CookiesLength && Cookies[i] != ';'; i++)
                if (Cookies[i] != ' ')
                    _value += Cookies[i];
            _cookies[_key] = _value;
        }
        _headers.erase("Cookie");
        _headers.erase("Cookies");
    }
}

void Request::parsseBody(size_t &_bodyPos)
{
    if (_headers.find("Content-Length") != _headers.end())
        _body = _request.substr(_bodyPos + 4 , std::atoi(_headers["Content-Length"].c_str()));
    std::cout <<  _body << std::endl;
}

Request::~Request()
{
    _headers.clear();
    _queries.clear();
    _cookies.clear();
}