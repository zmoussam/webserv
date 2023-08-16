#include "Request.hpp"


std::string Request::getFullRequest() const
{
    return this->_request;
}

size_t Request::getRequestLength() const
{
    return this->_requestLength;
}

std::string Request::getBody() const
{
    return this->_body;
}

std::string Request::getHTTPVersion() const
{
    return this->_httpVersion;
}

std::string Request::getPath() const
{
    return this->_URI;
}

std::string Request::getMethod() const
{
    return this->_method;
}

std::string Request::getQueries() const
{
    return this->_queries;
}

bool Request::KeepAlive() const
{
    return this->_keepAlive;
}

std::map<std::string, std::string> Request::getHeaders() const
{
    return this->_headers;
}

std::map<std::string, std::string> Request::getCookies() const
{
    return this->_cookies;
}

bool Request::isHeadersRead() const {
	return _isHeadersRead;
}

bool Request::isBodyRead() const {
	return _isBodyRead;
}

size_t getBodyLength(std::string Content_length)
{
    std::string bodylength = "";
    int i = 0;
    while (Content_length[i] != '\r' && std::isdigit(Content_length[i]))
    {
        bodylength += Content_length[i];
        i++;
    }
    return std::atoi(bodylength.c_str());
}

int hexStringToInt(const std::string hexString) {
    std::istringstream converter(hexString);
    int intValue;
    converter >> std::hex >> intValue;
    return intValue;
}