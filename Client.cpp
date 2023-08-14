#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Set up the server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000); // HTTP port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to server." << std::endl;
        close(clientSocket);
        return 1;
    }

    // Send an HTTP GET request
    const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	 int bytesSent = 0;
	while (bytesSent < strlen(request))
	{
		send(clientSocket, &request[bytesSent], 5, 0);
		std::cout << "bytesSent: " << bytesSent << std::endl;
		bytesSent += 5;
		sleep(1);
	}


    // Receive and print the response
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Error receiving data." << std::endl;
        close(clientSocket);
        return 1;
    }

    std::cout << "Response:" << std::endl;
    std::cout.write(buffer, bytesRead);

    // Clean up and close the socket
    close(clientSocket);

    return 0;
}
