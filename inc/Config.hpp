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
#define CONF_FILE "configs/file_config.conf"
#define NOT_VALID "not a valid line"
#define FAIL_OPEN "Failed to open the input file."
#define UNEXPECTED_TOKEN "Unexpected token"
#define LISTEN "listen"
#define SERVER_NAME "server_name"
#define ERROR_PAGE "error_page"
#define ROOT "root"
#define LOCATION "location"
#define AUTO_INDEX "autoindex"
#define ALLOW_METHODS "allow_methods"
#define INDEX "index"
#define HOST "host"
#define BODY_SIZE "client_body_size"
#define AUTO_INDEX "autoindex"
#define REDIRECT "return"

class ServerConf;
class Config {
    public:
        char *_file;
        Config();
        Config(char *file);
        ~Config();
        std::vector<ServerConf>  _servers;
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
    public:
        ServerConf();
        ServerConf(const ServerConf &copy);
        ServerConf &operator=(const ServerConf &copy);
        ~ServerConf();
        bool getAutoindex() const; // Return value: True || False
        size_t getNum(std::string type) const; // Takes in the Listen and Client_body_size and retruns the value of int type
        std::string getString(std::string type) const;
        void setString(std::string type, std::string value);
        void setNum(std::string type, size_t num);
        std::map<int , std::string> getErrorPages() const;
        void setErrorPage(std::map<int , std::string> error_pages);
        void setAutoindex(bool value);
        std::vector<Location> location;

};

class Location: public ServerConf {
    private:
        std::string _locationName;
        std::string _returned;
        std::vector<std::string> _methods;
    public:
        Location();
        ~Location();
        std::string getLocationName() const;
        std::vector<std::string> getMethods() const;
        std::string getReturned() const;
        void setReturned(std::string returned);
        void setLocationName(std::string location_name);
        void setMethods(std::vector<std::string> methods);
};
void parsefile(Config &config);