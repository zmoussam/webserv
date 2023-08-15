#include"Config.hpp"

Config::Config() {}
Config::Config(char *file)
{
    this->_file = file;
}
Config::~Config() {}
ServerConf::ServerConf() {
    this->_autoindex = false;
    this->_bodySize = 100000;
    this->_host = "127.0.0.1";
}
ServerConf::~ServerConf() {}
Location::Location() {}
Location::~Location() {}

void Location::setMethods(std::vector<std::string> methods)
{
    this->_methods = methods;
}

std::vector<std::string> Location::getMethods() const
{
    return this->_methods;
}
std::string Location::getLocationName() const
{
    return this->_locationName;
}

bool ServerConf::getAutoindex() const
{
    return this->_autoindex;
}

void ServerConf::setAutoindex(bool autoindex)
{
    this->_autoindex = autoindex;
}

void Location::setLocationName(std::string locationName)
{
    this->_locationName = locationName;
}

void ServerConf::setErrorPage(std::map<int , std::string>  errorPage) {
    this->_errorPages = errorPage;
}

std::map<int , std::string> ServerConf::getErrorPages() const {
    return this->_errorPages;
}


void ServerConf::setString(std::string type, std::string value)
{
    if (type == "server_name")
        this->_serverName = value;
    else if (type == "host")
        this->_host = value;
    else if (type == "root")
        this->_root = value;
    else if (type == "index")
        this->_index = value;
}

void ServerConf::setNum(std::string type, size_t value)
{
    if (type == "listen")
        this->_listen = value;
    else if (type == "client_body_size")
        this->_bodySize = value;
}

std::string ServerConf::getString(std::string type) const
{
    if (type == "server_name")
        return this->_serverName;
    else if (type == "host")
        return this->_host;
    else if (type == "root")
        return this->_root;
    else if (type == "index")
        return this->_index;
    return "";
}

size_t ServerConf::getNum(std::string type) const
{
    if (type == "listen")
        return this->_listen;
    else if (type == "client_body_size")
        return this->_bodySize;
    return 0;
}
