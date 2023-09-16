#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <ctype.h>
#include "Macros.hpp"

class ServerConf;
class Config {
    public:
        char *_file;
        Config();
        Config(char *file);
        ~Config();
        std::vector<ServerConf>  _servers;
        std::map<int , std::vector<ServerConf> > _serversByPort;
};

class Location;
class ServerConf {
    private:
        size_t _bodySize;
        std::string _host;
        std::string _serverName;
        size_t _listen;
    protected:
        bool _autoindex;
        std::string _root;
        std::string _index;
        std::map<int , std::string> _errorPages;
        std::vector<std::string> _methods;

    public:
        ServerConf();
        ServerConf(const ServerConf &copy);
        ServerConf &operator=(const ServerConf &copy);
        ~ServerConf();
        bool getAutoindex() const;
        size_t getNum(std::string type) const;
        std::vector<std::string> getMethods() const;
        std::string getString(std::string type) const;
        std::map<int , std::string> getErrorPages() const;
        void setAutoindex(bool value);
        void setNum(std::string type, size_t num);
        void setString(std::string type, std::string value);
        void setErrorPage(std::map<int , std::string> error_pages);
        void setMethods(std::vector<std::string> methods);
        std::vector<Location> location;

};

class Location: public ServerConf {
    private:
        std::string _locationName;
        std::string _returned;
        std::string _compiler;
    public:
        Location();
        ~Location();
        std::string getReturned() const;
        std::string getCompiler() const;
        std::string getLocationName() const;
        void setReturned(std::string returned);
        void setCompiler(std::string compiler);
        void setLocationName(std::string location_name);
};
void parsefile(Config &config);