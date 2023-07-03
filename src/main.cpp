#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

const int BUFFER_SIZE = 1024;

std::map<std::string, std::string> createMimeTypesMap() {
    std::map<std::string, std::string> mimeTypes;
    
    mimeTypes[".html"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "text/javascript";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".ico"] = "image/x-icon";
    mimeTypes[".mp4"] = "video/mp4";
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".ttf"] = "font/ttf";
    mimeTypes[".otf"] = "font/otf";
    mimeTypes[".woff"] = "font/woff";
    mimeTypes[".woff2"] = "font/woff2";
    mimeTypes[".eot"] = "application/vnd.ms-fontobject";
    mimeTypes[".sfnt"] = "application/font-sfnt";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".zip"] = "application/zip";

    return mimeTypes;
}

std::string getType(const std::string& path) {
    size_t dotPos = path.find_last_of('.');
	const std::map<std::string, std::string> mimeTypes = createMimeTypesMap();

    if (dotPos == std::string::npos)
        return "";
    
    std::string extension = path.substr(dotPos);
    std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
    if (it != mimeTypes.end())
        return it->second;
    
    return "";
}

std::string readFile(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void sendResponse(int clientSocket, const std::string& response) {
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
    if (bytesSent < 0) {
        std::cerr << "Error sending response" << std::endl;
    }
    close(clientSocket);
}

void handleClientRequest(int clientSocket) {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);

    ssize_t bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    if (bytesRead < 0) {
        std::cerr << "Error reading client request" << std::endl;
        return;
    }

    std::string request(buffer);
    std::istringstream requestStream(request);
    std::string method, path;
    requestStream >> method >> path;

    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;

	if (path == "/") {
		path = "/index.html";
	}
    if (method != "GET") {
        std::cerr << "Unsupported HTTP method: " << method << std::endl;

        std::string response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: 3\r\n\r\n405";
        sendResponse(clientSocket, response);
        return;
    }

    if (path.empty() || path[0] != '/') {
        std::cerr << "Invalid file path: " << path << std::endl;

        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 3\r\n\r\n404";
        sendResponse(clientSocket, response);
        return;
    }

    std::string adjustedPath = path.substr(1);
    std::string type = getType(adjustedPath);
    if (type.empty()) {
        std::cerr << "Unsupported file type: " << adjustedPath << std::endl;

        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 3\r\n\r\n404";
        sendResponse(clientSocket, response);
        return;
    }

    std::string file = readFile(adjustedPath);
    if (file.empty()) {
        std::cerr << "Error reading file: " << adjustedPath << std::endl;

        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 3\r\n\r\n404";
        sendResponse(clientSocket, response);
        return;
    }

    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + type + "\r\nContent-Length: " + std::to_string(file.length()) + "\r\n\r\n" + file;
    sendResponse(clientSocket, response);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) {
        std::cerr << "Error creating server socket" << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        return 1;
    }

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error binding server socket" << std::endl;
        return 1;
    }

    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Error listening for connections" << std::endl;
        return 1;
    }

    std::cout << "Server is listening on port http://127.0.0.1:8080" << std::endl;
    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            std::cerr << "Error accepting client connection" << std::endl;
            continue;
        }

        handleClientRequest(clientSocket);
    }

    close(serverSocket);

    return 0;
}
