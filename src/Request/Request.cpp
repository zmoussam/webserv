#include "Request.hpp"
#include <fstream>

// wait for the body to be read
int Request::waitForBody(size_t headerlength)
{
    size_t bodyLengthPos = _REQ.str().find("Content-Length");
    size_t chunkedPos = _REQ.str().find("Transfer-Encoding:");
    if (bodyLengthPos != std::string::npos) // content length request
    {
        // check if the body is read
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
    - chunkedPos).find("chunked") != std::string::npos) // chunked request
    { 
        // check if the body is read
        if (_REQ.str().find("\r\n0\r\n\r\n", chunkedPos) != std::string::npos)
        {
            _isBodyRead = true;
            _requestLength = _REQ.str().size();
            _request = _REQ.str();
            return DONE;
        }
    }
    else // no body
    {   
        _isBodyRead = true;
        _requestLength = _REQ.str().size();
        _request = _REQ.str();
        return DONE;
    }
    return (0);
}

// Receive the request from the client
int Request::recvRequest() {
	char buffer[1024] = {0};
    size_t headerlength = 0;
	int readRes = recv(_clientSocket, buffer, 1024, 0);
    // if recv() failed
	if (readRes == -1) {
		return DONE;
	}
    // if client disconnected
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
    // if the request is received and the body is read
	if (rcvRes == DONE && _isBodyRead) {
		parsseRequest(); // parse the request received from the client and fill the request object
        std::cout << "- - " << this->_method << " " << this->_URI << " " << this->_httpVersion << std::endl;
	}
	return (0);
}

Request::Request(int clientSocket, std::vector<ServerConf> servers) :
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
	_cookies(""),
    _config(),
    _servers(servers),
    _bodySize(0),
    _error(0),
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
    _cookies(""),
    _config(),
    _servers(),
    _bodySize(0),
    _error(0),
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
    parsseMethod(nextPos); // parse the method of the request (GET, POST, DELETE)
    parssePath_Queries(nextPos); // parse the path and the queries of the request
    parsseHTTPversion(nextPos); // parse the HTTP version of the request
    parsseHeaders(nextPos); // parse the headers of the request and fill the headers map
    parsseBody(nextPos); // parse the body of the request and fill the body string
    // parsseCookies(); // parse the cookies of the request and fill the cookies map 

}

void Request::parsseMethod(size_t &methodPos)
{
    // loop until the method is read or the end of the request is reached
    for (; methodPos < _requestLength && _request[methodPos] != ' '; methodPos++)
        _method += _request[methodPos];
}

void Request::parssePath_Queries(size_t &URI_Pos)
{
    
    std::string URI = "";
    size_t queryPos;
    // skip the spaces
    for (; URI_Pos < _requestLength && _request[URI_Pos] == ' '; URI_Pos++);
    // loop until the path is read or the end of the request is reached
    for (; URI_Pos < _requestLength && _request[URI_Pos] != ' '; URI_Pos++)
        URI += _request[URI_Pos];
    queryPos = URI.find('?'); // check if the path contains queries
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
    // skip the spaces
    for (; _httpversion_Pos < _requestLength && _request[_httpversion_Pos] == ' '; _httpversion_Pos++);
    // loop until the HTTP version is read or the end of the request is reached
    for (; _httpversion_Pos < _requestLength && _request[_httpversion_Pos] != ' ' \
       && _request[_httpversion_Pos] != '\r'; _httpversion_Pos++)
        _httpVersion += _request[_httpversion_Pos];
}

void Request::parsseHeaders(size_t &_hpos)
{
    std::string _key;
    size_t _headerkeyPos;
    size_t _headerValuePos;
    _keepAlive = false; // set the keep alive to false by default (if the connection header is not found)
    size_t bodypos = _request.find("\r\n\r\n", _hpos); // find the end of the headers
    if (bodypos == std::string::npos)
        return;
    // loop until the end of the headers is reached
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
        _hpos = _headerValuePos; // increment the position of the headers

        // use this comment to print the headers of the request (for debugging)
        // std::cout << '$' << _key << "$ : " << '$' << this->_headers[_key]  << "$" << std::endl;
    }
    if (_headers.find("Cookie") != _headers.end()) { 
        _cookies = _headers["Cookie"];
        _headers.erase("Cookie");
    }
    else if(_headers.find("Cookies") != _headers.end()) {
        _cookies = _headers["Cookies"];
        _headers.erase("Cookies");
    }
    else 
        _cookies = "";

}

// parse the cookies of the request and fill the cookies map
// void Request::parsseCookies()
// {
//     std::string _key;
//     std::string _value;
//     std::string Cookies;
//     // check if the cookies are found in the headers map (the key can be "Cookie" or "Cookies" depending on the browser)
//     if (_headers.find("Cookie") != _headers.end() || _headers.find("Cookies") != _headers.end()) {
//         if (_headers.find("Cookie") != _headers.end()) { Cookies = _headers["Cookie"]; }
//         else { Cookies = _headers["Cookies"]; }
//         size_t CookiesLength = Cookies.size();
//         // loop until the end of the cookies is reached
//         for (size_t i = 0; i < CookiesLength; i++) {
//             _key = "" , _value = "";
//             // loop until the end of the key is reached
//             for (; i < CookiesLength && Cookies[i] != '='; i++)
//                 if (Cookies[i] != ' ')
//                     _key +=  Cookies[i];
//             i++;
//             // loop until the end of the value is reached
//             for (; i < CookiesLength && Cookies[i] != ';'; i++)
//                 if (Cookies[i] != ' ')
//                     _value += Cookies[i];
//             _cookies[_key] = _value; // add the cookie to the cookies map 
//         }
//         // move the cookies from the headers map
//         _headers.erase("Cookie");
//         _headers.erase("Cookies");
//     }
// }

// count the number of boundaries in the request
size_t Request::countboundaries(size_t pos)
{
    size_t count = 0;
    while ((pos = _request.find(_boundary, pos)) != std::string::npos)
    {
        count++;
        pos += _boundary.length();
    }
    return count;
}
// get the headers of the boundary body
std::map<std::string, std::string> getboundaryHeaders(std::string headers)
{
    std::map<std::string, std::string> _headers;
    std::string _key;
    size_t _headerkeyPos;
    size_t _headerValuePos;
    size_t _hpos = 0;
    size_t headersLength = headers.size();
    // loop until the end of the headers is reached
    while (_hpos < headersLength)
    {
        _headerkeyPos = headers.find(':', _hpos);
        if (_headerkeyPos == std::string::npos)
            break;
        _key = headers.substr(_hpos, _headerkeyPos - _hpos);
        if ((_headerValuePos = headers.find("\r\n", _headerkeyPos)) == std::string::npos)
            return _headers;
        _headers[_key] = headers.substr(_headerkeyPos + 2, _headerValuePos - _headerkeyPos - 2);
        _hpos = _headerValuePos + 2;
    }
    return _headers;
}
void Request::getChunkedBody(size_t &_bodyPos)
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
        int chunkIntValue = hexStringToInt(chunkedSize); // convert the chunk size from hex to int
        if (chunkIntValue == 0)
            break;
        _body += tmp.substr(i, chunkIntValue);
        i += chunkIntValue + 1;
    }
}
// find the config of the request if there is multiple servers
void    Request::findConfig()
{
    // check if the host is found in the headers map
    if (_headers.find("Host") != _headers.end())
    {
        std::string host = _headers["Host"];
        for (std::vector<ServerConf>::iterator it = _servers.begin(); it != _servers.end(); it++)
        {
            // check if the host is found in the servers vector
            if (it->getString(SERVER_NAME) == host)
            {
                _config = *it;
                return;
            }
        }
    }
    _config = _servers[0];
}

void Request::creatUploadFile(BoundaryBody *headBoundaryBody)
{
    this->findConfig();
    size_t filenamePos = headBoundaryBody->headers["Content-Disposition"].find("filename");
    if (filenamePos == std::string::npos)
    {
        headBoundaryBody->filename = "";
        headBoundaryBody->_isFile = false;
    }
    else if (_bodySize <= _config.getNum(BODY_SIZE))
    {

        headBoundaryBody->filename = headBoundaryBody->headers["Content-Disposition"].substr(filenamePos + 10, \
        headBoundaryBody->headers["Content-Disposition"].length() - filenamePos - 11);
        headBoundaryBody->_isFile = true;
        // std::ofstream file((_config.getString(UPLOAD_PATH) + headBoundaryBody->filename).c_str());
        // if (!file) {
        //     std::cout << "Failed to open the file!" << std::endl;
        // }
        // else 
        // {
        //     file << headBoundaryBody->_body;
        //     file.close();
        // }
    }
    else 
        _error = 413;
}

void Request::getBoundaries(size_t &_bodyPos)
{
    _boundary = "--" + _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary") + 9);
    size_t bodyCount = countboundaries(_bodyPos);
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
        creatUploadFile(headBoundaryBody); // create the file if the body is a file
        bodyCount--;
        headBoundaryBody = headBoundaryBody->next;
        _bodyPos += tmpbody.length() + _boundary.length() + 2;
    }
}

void Request::parsseBody(size_t &_bodyPos)
{
    // check if the body is found in the headers map and if the body is not empty and not chunked
    if (_headers.find("Content-Length") != _headers.end())
    {  
        std::istringstream iss(_headers["Content-Length"]);
        if (!(iss >> _bodySize)) // convert the body size from string to long long
            _bodySize = 0;
        // check if the body is a multipart/form-data request and if the boundary is found in the headers map
        if(_headers.find("Content-Type") != _headers.end() \
        && _headers["Content-Type"].find("multipart/form-data") != std::string::npos \
        && _headers["Content-Type"].find("boundary") != std::string::npos)
        {
            // get the boundaries of the body and fill the boundary body linked list
            getBoundaries(_bodyPos);
        }
        // check if the body is a urlencoded request
        else
        _body = _request.substr(_bodyPos + 4 , _bodySize);
    }
    else if (_headers.find("Transfer-Encoding") != _headers.end() \
    && _headers["Transfer-Encoding"].find("chunked") != std::string::npos) // chunked request body
    {
        getChunkedBody(_bodyPos);
        _bodySize = _body.size();
    }
    // std::cout <<_body << std::endl;
}
void freeBoundaryBody(BoundaryBody *head)
{
    BoundaryBody *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        delete tmp;
    }
}
Request::~Request()
{
    // std::cout << "Request destroyed" << std::endl;
    freeBoundaryBody(_boundaryBody);
    _headers.clear();
    _queries.clear();
    _cookies.clear();
}