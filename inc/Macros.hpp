#pragma once

#define CONF_FILE "configs/file_config.conf"
#define NOT_VALID "not a valid line"
#define BIG_NUMBER "number is too big"
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
#define COMPILER "compiler"
#define UPLOAD_PATH "upload_path"

#define unused(x) (void)(x)
#define ERROR -1
#define SUCCESS 0
#define TRUE 1
#define FALSE 0
#define DONE 2
#define CONTINUE 3
#define DISCONNECTED 4
#define BUFSIZE 1024
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