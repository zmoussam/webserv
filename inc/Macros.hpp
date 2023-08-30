#pragma once


#define unused(x) (void)(x)
#define ERROR -1
#define SUCCESS 0
#define TRUE 1
#define FALSE 0
#define DONE 2
#define CONTINUE 3
#define DISCONNECTED 4
#define BUFSIZE 1024
// #define ROOT_PATH "./www"
# include <string>
# include <map>

extern std::map<std::string, std::string> mimeTypes;

enum HttpStatusCode {
    OK = 200,
    CREATED = 201,
    NO_CONTENT = 204,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503
};