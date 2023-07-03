#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int BUFFER_SIZE = 1024;

void handleClientRequest(int clientSocket)
{
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);

    ssize_t bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    if (bytesRead < 0) {
        std::cerr << "Error reading client request" << std::endl;
        return;
    }

    char *spacePos = std::strchr(buffer, ' ');
    if (spacePos == nullptr) {
        std::cerr << "Invalid request format" << std::endl;
        return;
    }
    *spacePos = '\0';
    char *method = buffer;

    char *secondSpacePos = std::strchr(spacePos + 1, ' ');
    if (secondSpacePos == nullptr) {
        std::cerr << "Invalid request format" << std::endl;
        return;
    }
    *secondSpacePos = '\0';
    char *path = spacePos + 1;

    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;

    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Server: Webserv\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "<center><h1>Webserv testing page</h1></center>\r\n";
    ssize_t bytesSent = send(clientSocket, response, std::strlen(response), 0);
    if (bytesSent < 0) {
        std::cerr << "Error sending response" << std::endl;
        return;
    }

    close(clientSocket);
}

int main()
{
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
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        return 1;
    }

    if (bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
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

        int clientSocket = accept(serverSocket, (sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            std::cerr << "Error accepting client connection" << std::endl;
            continue;
        }

        handleClientRequest(clientSocket);
    }

    close(serverSocket);

    return 0;
}