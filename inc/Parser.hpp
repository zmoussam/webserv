#pragma once
#include "Config.hpp"

typedef struct t_token{
	std::string _type;
	std::string _value;
}				Token;

class ServerConf;
class Config;
class Location;
class Parser {
    private:
        size_t index;
        ServerConf parseServer();
        bool parseAutoindex();
        size_t parseNumRules(std::string type);
        std::string parseStringRules(std::string type);
        std::map<int , std::string> parseErrorPage();
        std::string parseReturned();
        std::vector<std::string> parseMethods();
        ServerConf parseServerBody();
        Location parseLocation();
        Location parseLocationBody();
        std::vector<Token> tokens;
    public:
        Parser();
        ~Parser();
        Config parseConfig();
        Config parseToken(void);
        bool look(std::string type);
        Token match(std::string type);
};
