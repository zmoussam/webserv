# include "Server.hpp"
# include <vector>
# include <poll.h>
# include <map>
# include <ctime>
# include <fcntl.h>

std::map<std::string, std::string> mimeTypes = getMimeTypes();

/**
 * Constructor for the Server class.
 * Initializes the server socket and sets up the server address.
 *
 * @param address The server address to bind to.
 * @param port The port number to listen on.
 */
Server::Server(int port) {
    int reuse = 1;
    
    _port = port;
    _serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_serverSocket < 0) {
        std::cerr << "Error: socket() failed" << strerror(errno) << std::endl;
        return;
    }
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    _serverAddress.sin_port = htons(port);

    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
        std::cerr << "Error: setsockopt() failed" << std::endl;
        return;
    }

    if (bind(_serverSocket, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) < 0) {
        std::cerr << "Error: bind() failed" << std::endl;
        return;
    }
}

/**
 * Destructor for the Server class.
 * Closes the server socket.
 */
Server::~Server() {
    close(_serverSocket);
}

void Server::setPort(int port) {
	_port = port;
}

int Server::getPort() const {
	return _port;
}

/**
 * Starts the server by listening for incoming client connections.
 * Handles client requests in an infinite loop.
 */
int Server::start(void) {
    std::vector<int> clients;
    std::map<int, Request> requests;
    std::map<int, Response> responses;

    if (listen(_serverSocket, 1024) < 0) {
        std::cerr << "Error: listen() failed" << std::endl;
        return ERROR;
    }

    fd_set masterSet;
    FD_ZERO(&masterSet);
    FD_SET(_serverSocket, &masterSet);
    int maxFd = _serverSocket;

    std::cout << "Server started on port: " << _port << std::endl;
    int req = 0;
    int res = 0;
    while (true) {
        fd_set readSet = masterSet;
        fd_set writeSet = masterSet;
        int selectRes = select(maxFd + 1, &readSet, &writeSet, NULL, NULL);

        if (selectRes < 0) {
            std::cerr << "Error: select() failed" << std::endl;
            return ERROR;
        }

        // if client is ready to be read from accept the connection
        if (FD_ISSET(_serverSocket, &readSet)) {
            int clientSocket = accept(_serverSocket, NULL, NULL);
            if (clientSocket < 0) {
                std::cerr << "Error: accept() failed" << std::endl;
                continue;
            }
            // std::cout << "New client connected with socket: " << clientSocket << std::endl;
            clients.push_back(clientSocket);
            FD_SET(clientSocket, &masterSet);
            if (clientSocket > maxFd) {
                maxFd = clientSocket;
            }
            requests[clientSocket] = Request(clientSocket);
            responses[clientSocket] = Response(clientSocket);
        }
        // iterate through the clients and just print the request
        for (size_t i = 0; i < clients.size(); i++) {
            int clientSocket = clients[i];

            if (FD_ISSET(clientSocket, &readSet)) {
                req = requests[clientSocket].handleRequest();        
                if (req == DISCONNECTED) {
                    close(clientSocket);
                    FD_CLR(clientSocket, &masterSet);
                    clients.erase(clients.begin() + i);
                    responses.erase(clientSocket);
                    requests.erase(clientSocket);
                    i--;
                    req = 0;
                }
            }
            
            if (FD_ISSET(clientSocket, &writeSet) && requests[clientSocket].isHeadersRead()) {
                res = responses[clientSocket].sendResp(requests[clientSocket]);
            }
            if (res == DONE && requests[clientSocket].isHeadersRead()) {
                close(clientSocket);
                FD_CLR(clientSocket, &masterSet);
                clients.erase(clients.begin() + i);
                responses.erase(clientSocket);
                requests.erase(clientSocket);
                i--;
                res = 0;
            }
        }
    }
}