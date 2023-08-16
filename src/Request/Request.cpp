/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmoussam <zmoussam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 21:46:08 by zmoussam          #+#    #+#             */
/*   Updated: 2023/08/16 14:27:11 by zmoussam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

int Request::waitForBody(size_t headerlength)
{
    size_t bodyLengthPos = _request.find("Content-Length");
    size_t chunkedPos = _request.find("Transfer-Encoding:");
    if (bodyLengthPos != std::string::npos)
    {
        size_t bodyLength = getBodyLength(_request.substr(bodyLengthPos + 16, \
        _request.find("\r\n", bodyLengthPos + 16) - bodyLengthPos - 16));
        if (_request.substr(headerlength + 4).size() == bodyLength)
        {
            _isBodyRead = true;
            _requestLength = _request.size();
            return DONE;
        }
    }
    else if (chunkedPos != std::string::npos \
    && _request.substr(chunkedPos, _request.find("\r\n", chunkedPos) \
    - chunkedPos).find("chunked") != std::string::npos)
    {
        if (_request.find("\r\n0\r\n\r\n", chunkedPos) != std::string::npos)
        {
            // std::cout << "chunked" << std::endl;
            _isBodyRead = true;
            _requestLength = _request.size();
            return DONE;
        }
    }
    else
    {
        _isBodyRead = true;
        _requestLength = _request.size();
        return DONE;
    }
    return (0);
}

int Request::recvRequest() {
	char buffer[1024];
    size_t headerlength = 0;
	int readRes = recv(_clientSocket, buffer, 1023, 0);
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
    headerlength = _request.find("\r\n\r\n");
	if (headerlength != std::string::npos && !_isBodyRead) {
		_isHeadersRead = true;
        return waitForBody(headerlength);
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
	if (rcvRes == DONE && _isBodyRead) {
		parsseRequest();
	    // std::cout << " - - " << "\"" << _method << " " << _URI << " " << _httpVersion << "\"" << std::endl;
        // std::cout << _request << std::endl;
	}
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
	_isHeadersRead(false),
    _clientSocket(clientSocket),
    _isBodyRead(false)
{
}

Request::Request() :
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
    _isHeadersRead(false),
    _clientSocket(-1),
     _isBodyRead(false)
{
}

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
    else if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"].find("chunked") != std::string::npos)
    {
        std::string tmp = _request.substr(_bodyPos + 4);
        size_t _bodySize = tmp.size();
        for(size_t i = 0; i < _bodySize; i++)
        {
            std::string chunkedSize = "";
            size_t j = i;
            for (; tmp[j] != '\r'; j++)
                chunkedSize += tmp[j];
            i = j + 2;
            int chunkIntValue = hexStringToInt(chunkedSize);
            if (chunkIntValue == 0)
                break;
            _body += tmp.substr(i, chunkIntValue);
            i += chunkIntValue + 1;
        }
    }
    std::cout << _body << std::endl;
}

Request::~Request()
{
    _headers.clear();
    _queries.clear();
    _cookies.clear();
}
