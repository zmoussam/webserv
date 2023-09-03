/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aarbaoui <aarbaoui@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 21:46:08 by zmoussam          #+#    #+#             */
/*   Updated: 2023/09/03 19:01:37 by aarbaoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <fstream>
int Request::waitForBody(size_t headerlength)
{
    size_t bodyLengthPos = _REQ.str().find("Content-Length");
    size_t chunkedPos = _REQ.str().find("Transfer-Encoding:");
    if (bodyLengthPos != std::string::npos)
    {
        size_t bodyLength = getBodyLength(_REQ.str().substr(bodyLengthPos + 16, \
        _REQ.str().find("\r\n", bodyLengthPos + 16) - bodyLengthPos - 16));
        std::string  body = _REQ.str().substr(headerlength + 4);
        if (body.size() == bodyLength)
        {
            _isBodyRead = true;
            _requestLength = _REQ.str().size();
            _request = _REQ.str();
            return DONE;
        }
    }
    else if (chunkedPos != std::string::npos \
    && _REQ.str().substr(chunkedPos, _REQ.str().find("\r\n", chunkedPos) \
    - chunkedPos).find("chunked") != std::string::npos)
    {
        if (_REQ.str().find("\r\n0\r\n\r\n", chunkedPos) != std::string::npos)
        {
            _isBodyRead = true;
            _requestLength = _REQ.str().size();
            _request = _REQ.str();
            return DONE;
        }
    }
    else
    {
        _isBodyRead = true;
        _requestLength = _REQ.str().size();
        _request = _REQ.str();
        return DONE;
    }
    return (0);
}

int Request::recvRequest() {
	char buffer[1024] = {0};
    size_t headerlength = 0;
	int readRes = recv(_clientSocket, buffer, 1024, 0);
	if (readRes == -1) {
		std::cerr << "Error: recv() failed" << std::endl;
		return DONE;
	}
	else if (readRes == 0) {
		std::cout << "Client disconnected" << std::endl;
		return DISCONNECTED;
	}
	buffer[readRes] = '\0';
    this->_REQ.write(buffer, readRes);
    headerlength = _REQ.str().find("\r\n\r\n");
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
    _REQ(),
	_request(""),
    _requestLength(0),
    _httpVersion(""),
	_body(""),
	_URI(""),
    _method(""),
	_queries(),
    _boundary(""),
    _boundaryBody(NULL),
	_headers(),
	_cookies(),
	_keepAlive(1),
	_isHeadersRead(false),
    _clientSocket(clientSocket),
    _isBodyRead(false),
    _checkBoundary(false)
{
}

Request::Request() :
    _REQ(),
    _request(""),
    _requestLength(0),
    _httpVersion(""),
    _body(""),
    _URI(""),
    _method(""),
    _queries(),
    _boundary(""),
    _boundaryBody(NULL),
    _headers(),
    _cookies(),
    _keepAlive(1),
    _isHeadersRead(false),
    _clientSocket(-1),
     _isBodyRead(false),
     _checkBoundary(false)
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
size_t Request::countboundary(size_t pos)
{
    size_t count = 0;
    while ((pos = _request.find(_boundary, pos)) != std::string::npos)
    {
        count++;
        pos += _boundary.length();
    }
    return count;
}
std::map<std::string, std::string> getboundaryHeaders(std::string headers)
{
    // std::cout << "headers : " << headers << std::endl;
    std::map<std::string, std::string> _headers;
    std::string _key;
    size_t _headerkeyPos;
    size_t _headerValuePos;
    size_t _hpos = 0;
    while (_hpos < headers.size())
    {
        _headerkeyPos = headers.find(':', _hpos);
        if (_headerkeyPos == std::string::npos)
            break;
        _key = headers.substr(_hpos, _headerkeyPos - _hpos);
        if ((_headerValuePos = headers.find("\r\n", _headerkeyPos)) == std::string::npos)
            return _headers;
        _headers[_key] = headers.substr(_headerkeyPos + 2, _headerValuePos - _headerkeyPos - 2);
        _hpos = _headerValuePos + 2;
        std::cout << '$' << _key << "$ : " << '$' << _headers[_key]  << "$" << std::endl;
    }
    return _headers;
}

void Request::parsseBody(size_t &_bodyPos)
{
    if (_headers.find("Content-Length") != _headers.end())
    {
        if(_headers.find("Content-Type") != _headers.end() 
        && _headers["Content-Type"].find("multipart/form-data") != std::string::npos
        && _headers["Content-Type"].find("boundary") != std::string::npos)
        {
            _boundary = "--" + _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary") + 9);
            size_t bodyCount = countboundary(_bodyPos);
            _bodyPos += 4 + _boundary.length() + 2;
            BoundaryBody *headBoundaryBody = new BoundaryBody;
            _boundaryBody = headBoundaryBody;
            while (bodyCount > 1 && headBoundaryBody != NULL)
            {
                std::string tmpbody = _request.substr(_bodyPos, _request.find(_boundary, _bodyPos) - _bodyPos);
                headBoundaryBody->_body = tmpbody.substr(tmpbody.find("\r\n\r\n") + 4);
                headBoundaryBody->headers = getboundaryHeaders(tmpbody.substr(0, tmpbody.find("\r\n\r\n") + 4));
                if (bodyCount > 2)
                    headBoundaryBody->next = new BoundaryBody;
                else 
                    headBoundaryBody->next = NULL;
                size_t filenamePos = headBoundaryBody->headers["Content-Disposition"].find("filename");
                if (filenamePos == std::string::npos)
                {
                    headBoundaryBody->filename = "";
                    headBoundaryBody->_isFile = false;
                }
                else 
                {
                    headBoundaryBody->filename = headBoundaryBody->headers["Content-Disposition"].substr(filenamePos + 10, headBoundaryBody->headers["Content-Disposition"].length() - filenamePos - 11);
                    headBoundaryBody->_isFile = true;
                    std::ofstream file(("upload/" + headBoundaryBody->filename).c_str());
                    if (!file) {
                        std::cout << "Failed to open the file!" << std::endl;
                    }
                    file << headBoundaryBody->_body;
                    file.close();
                }
                bodyCount--;
                headBoundaryBody = headBoundaryBody->next;
                _bodyPos += tmpbody.length() + _boundary.length() + 2;
            }
        } 
        else
        _body = _request.substr(_bodyPos + 4 , std::atoi(_headers["Content-Length"].c_str()));
    }
    else if (_headers.find("Transfer-Encoding") != _headers.end() \
    && _headers["Transfer-Encoding"].find("chunked") != std::string::npos)
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
    // std::cout << _body << std::endl;
}
void freeBoundaryBody(BoundaryBody *head)
{
    BoundaryBody *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
    }
}
Request::~Request()
{
    std::cout << "Request destroyed" << std::endl;
    freeBoundaryBody(_boundaryBody);
    _headers.clear();
    _queries.clear();
    _cookies.clear();
}
