#include"Parser.hpp"

Parser::Parser() {
    index = 0;
}
Parser::~Parser() {}
bool Parser::look(std::string type) {
    if (index > tokens.size())
        return false;
    if (index == tokens.size())
        return false;
    return tokens[index]._type == type;
}

Token Parser::match(std::string type) {
    if (look(type)) {
        return tokens[index++];
    }
    throw std::runtime_error(UNEXPECTED_TOKEN);
}

bool checkQuotes(std::string &line)
{
    size_t count = 0;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '"') {
            count++;
        }
    }
    if (count % 2 != 0)
        throw std::runtime_error(NOT_VALID);
    return (count % 2 == 0);
}

size_t parseNum(std::string str) {
    size_t number = 0;
    if (str.empty())
        throw std::runtime_error(NOT_VALID);
    for (size_t i = 0; i < str.length(); i++) {
        if (isdigit(str[i]))
            number = number * 10 + (str[i] - '0');
        else
            throw std::runtime_error(NOT_VALID);
    }
    return number;
}

std::string parseValue(std::string str) {
    std::string res;
    if (str[0] == '"' && checkQuotes(str)) {
        res = str.substr(1, str.size() - 2);
    }
    else
        throw std::runtime_error(NOT_VALID);
    return res;
}

std::vector<std::string> splitArgs(std::string value)
{
    std::vector<std::string> args;
    size_t quoteStartPos = 0;
    size_t quoteEndPos = value.find('"', quoteStartPos + 1);
    size_t count = std::count(value.begin(), value.end(), ',');
    while (true)
    {
        if (count == 0 && value.find(',', quoteEndPos + 1) != std::string::npos)
            throw std::runtime_error(NOT_VALID);
        if (quoteStartPos != std::string::npos && quoteEndPos != std::string::npos)
        {
            std::string arg = value.substr(quoteStartPos + 1, quoteEndPos - quoteStartPos - 1);
            if (arg.find(' ') != std::string::npos || arg.empty()) {
                 throw std::runtime_error(NOT_VALID);
            }
            args.push_back(arg);
            quoteStartPos = value.find('"', quoteEndPos + 1);
            quoteEndPos = value.find('"', quoteStartPos + 1);
        }
        if (count == 0)
            break;
        count--;
    }
    return args;
}

std::map<int , std::string>  Parser::parseErrorPage() {
    match("error_page");
    std::map<int , std::string>  errorPage;
    std::vector<std::string> args;

    args = splitArgs(match("value")._value);
    if (args.size() % 2 != 0)
        throw std::runtime_error(NOT_VALID);
    for (size_t i = 0; i < args.size(); i++) {
        size_t code = parseNum(args[i++]);
        std::string path = args[i];
        errorPage[code] = path;
    }
    return errorPage;
}


std::string Parser::parseReturned()
{
    match("return");
    std::string returned = parseValue(match("value")._value);
    return returned;
}

bool Parser::parseAutoindex()
{
    match("autoindex");
    bool autoindex;
    std::string boolstr = parseValue(match("value")._value);
    if (boolstr == "on")
        autoindex = true;
    else if (boolstr == "off")
        autoindex = false;
    else
        throw std::runtime_error(NOT_VALID);
    return autoindex;
}

std::vector<std::string> Parser::parseMethods()
{
    match("allow_methods");
    std::vector<std::string> methods;
    std::vector<std::string> args;

    args = splitArgs(match("value")._value);
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "GET" || args[i] == "POST" || args[i] == "DELETE")
            methods.push_back(args[i]);
        else
            throw std::runtime_error(NOT_VALID);
    }
    return methods;
}

size_t Parser::parseNumRules(std::string key) {
    match(key);
    size_t bodySize = parseNum(parseValue(match("value")._value));
    return bodySize;
}

std::string Parser::parseStringRules(std::string key) {
    match(key);
    std::string value = parseValue(match("value")._value);
    return value;
}

Location Parser::parseLocationBody()
{
    Location location;

    location.setLocationName(parseValue(match("value")._value));
    match("{");
    while (!look("}")) {
        if (look("allow_methods"))
            location.setMethods(parseMethods());
        else if (look("root"))
            location.setString(ROOT, parseStringRules(ROOT));
        else if (look("index"))
            location.setString(INDEX, parseStringRules(INDEX));
        else if (look("client_body_size"))
            location.setNum(BODY_SIZE, parseNumRules(BODY_SIZE));
        else if (look("error_page"))
            location.setErrorPage(parseErrorPage());
        else if (look("autoindex"))
            location.setAutoindex(parseAutoindex());
        else if (look("return"))
            location.setReturned(parseStringRules(REDIRECT));
        else if (look("compiler"))
            location.setCompiler(parseStringRules(COMPILER));
        else
            throw std::runtime_error(UNEXPECTED_TOKEN);
    }
    return location;
}

Location Parser::parseLocation() {
    Location location;
    match("location");
    location = parseLocationBody();
    match("}");
    return location;
}

ServerConf Parser::parseServerBody() {
    ServerConf server;

    while (!look("}")) {
        if (look("listen"))
            server.setNum(LISTEN, parseNumRules(LISTEN)); 
        else if (look("host"))
            server.setString(HOST, parseStringRules(HOST));
        else if (look("server_name"))
            server.setString(SERVER_NAME, parseStringRules(SERVER_NAME));
        else if (look("root"))
            server.setString(ROOT, parseStringRules(ROOT));
        else if (look("index"))
            server.setString(INDEX, parseStringRules(INDEX));
        else if (look("client_body_size"))
            server.setNum(BODY_SIZE, parseNumRules(BODY_SIZE));
        else if (look("error_page"))
            server.setErrorPage(parseErrorPage());
        else if (look("autoindex"))
            server.setAutoindex(parseAutoindex());
        else if (look("allow_methods"))
        {
            server.setMethods(parseMethods());
        }
        else if (look("location"))
            server.location.push_back(parseLocation());
        else
            throw std::runtime_error(UNEXPECTED_TOKEN);
    }
    return server; 
}

ServerConf Parser::parseServer() {
    match("server");
    match("{");
    ServerConf server = parseServerBody();
    match("}");
    if (!look("server") && index != tokens.size())
        throw std::runtime_error(UNEXPECTED_TOKEN);
    return server;
}

void Parser::parseConfig(Config &config) {
    if (look("server"))
    {
        while (look("server"))
        {
            if (look("server"))
                config._servers.push_back(parseServer());
            else
                throw std::runtime_error(UNEXPECTED_TOKEN);
        }
    }
    else
        throw std::runtime_error(UNEXPECTED_TOKEN);
}

std::string parseKey(std::string token)
{
    std::string key;
    for (size_t i = 0; i < token.size(); ++i) {
        if (token[i] == ':') {
            key = token.substr(0, i);
            break;
        }
    }
    return key;
}

void   fillTokens(std::string line, Token tokens, std::vector<Token> &tokenArr)
{
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        if (!token.empty() && (!token.compare("server") || !token.compare("{") || !token.compare("}") || !token.compare("location"))) {
            tokens._type = token;
            tokens._value = token;
        }
        else if (!token.empty() && token[token.length() - 1] == ':')
        {
            tokens._type = parseKey(token);
            tokens._value = token;
        }
        else if (!token.empty() && token[0] == '"')
        {
            tokens._type = "value";
            tokens._value = token;
        }
        else
            throw std::runtime_error(NOT_VALID);
        tokenArr.push_back(tokens);
    }
}

void Tokenizer(std::vector<Token> &tokens, char *fileName)
{
    std::ifstream file(fileName);
    std::string line;
    Token token;
    if (file.is_open()) {
        while (getline(file, line)) {
            size_t start_pos = line.find_first_not_of(" \t");
            if (start_pos != std::string::npos)
                line = line.substr(start_pos);
            if (!line.empty())
                fillTokens(line, token, tokens);
        }
        file.close();
    }
    else
        throw std::runtime_error(FAIL_OPEN);
}

void Parser::parseToken(Config &config)
{
    try {
        Tokenizer(this->tokens, config._file);
        parseConfig(config);
        while (config._servers.size() > 1) {
            for (size_t i = 0; i < config._servers.size() - 1; i++) {
                for (size_t j = i + 1; j < config._servers.size(); j++) {
                    if (config._servers[i].getNum(LISTEN) == config._servers[j].getNum(LISTEN) && config._servers[i].getString(SERVER_NAME) == config._servers[j].getString(SERVER_NAME))
                        throw std::runtime_error("ERROR: SAME_SERVER");
                }
            }
            break;
        }   
        for (size_t i = 0; i < config._servers.size(); i++) {
            config._serversByPort[config._servers[i].getNum(LISTEN)].push_back(config._servers[i]);
        }
        //print to test ou sf
        // for (std::map<int , std::vector<ServerConf> >::iterator it = config._serversByPort.begin(); it != config._serversByPort.end(); it++) {
        //     std::cout << "Port: " << it->first << std::endl;
        //     std::vector<ServerConf>& servers = it->second;

        // for (size_t i = 0; i < servers.size(); ++i) {
        //     ServerConf& server = servers[i];
        //     std::cout << "Listen: " << server.getNum(LISTEN) << std::endl;
        //     std::cout << "Host: " << server.getString(HOST) << std::endl;
        //     std::cout << "Server Name: " << server.getString(SERVER_NAME) << std::endl;
        //     std::cout << "allow_methods: " << server.getErrorPages()[404] << std::endl;

        //     std::cout << "*****************"<< i<<"*******************" << std::endl;
        // }
    // }
        // for (size_t i = 0; i < config._servers.size(); i++) {
        //     std::cout << "listen: " << config._servers[i].getNum(LISTEN) << std::endl;
        //     std::cout << "host: " << config._servers[i].getString(HOST) << std::endl;
        //     std::cout << "server_name: " << config._servers[i].getString(SERVER_NAME) << std::endl;
        //     for (size_t j = 0; j < config._servers[i].location.size(); j++) {
        //         std::cout << "location: " << config._servers[i].location[j].getLocationName() << std::endl;
        //         std::cout << "root: " << config._servers[i].location[j].getString(ROOT) << std::endl;
        //     }
        // }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
}
void parsefile(Config &config)
{
    Parser  parse;
    parse.parseToken(config);
}
