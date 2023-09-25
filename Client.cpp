#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

int main() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Set up the server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000);  // Replace with your server's port number
    inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr));  // Replace with your server's IP address

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to connect to the server." << std::endl;
        close(sockfd);
        return 1;
    }

    // Send a command to the server
    const char* command = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    ssize_t bytesSent = send(sockfd, command, strlen(command), 0);
    if (bytesSent == -1) {
        std::cerr << "Failed to send command to the server." << std::endl;
        close(sockfd);
        return 1;
    }

    // Close the socket
    close(sockfd);

    // Optionally, you can wait for a response from the server before closing the socket
    // Just uncomment the code below if you want to receive a reply from the server
    /*
    char buffer[1024];
    ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Failed to receive reply from the server." << std::endl;
        close(sockfd);
        return 1;
    }

    // Process the received data (e.g., print it)
    std::cout << "Server reply: " << std::string(buffer, bytesRead) << std::endl;
    */

    return 0;
}