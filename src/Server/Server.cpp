# include "Server.hpp"
# include <vector>
# include <poll.h>
# include <map>
# include <ctime>
/**
 * Constructor for the Server class.
 * Initializes the server socket and sets up the server address.
 *
 * @param address The server address to bind to.
 * @param port The port number to listen on.
 */
Server::Server(std::string address, int port) {
    int reuse = 1;
    
    _port = port;
    _serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_serverSocket < 0) {
        std::cerr << "Error: socket() failed" << std::endl;
        _exit(1);
    }
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_addr.s_addr = inet_addr(address.c_str());
    _serverAddress.sin_port = htons(port);

    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
        std::cerr << "Error: setsockopt() failed" << std::endl;
        _exit(1);
    }

    if (bind(_serverSocket, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) < 0) {
        std::cerr << "Error: bind() failed" << std::endl;
        _exit(1);
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
void Server::start(void) {
    std::vector<pollfd> clients;
    std::map<int, time_t> keepAliveClients;
    
    if (listen(_serverSocket, 1000) < 0) {
        std::cerr << "Error: listen() failed" << std::endl;
        _exit(1);
    }

    pollfd serverpollfd = { _serverSocket, POLLIN, 0 };
    clients.push_back(serverpollfd);
    std::cout << "Server started on " << "http://" << inet_ntoa(_serverAddress.sin_addr) << ":" << _port << std::endl;

    while (true) {
        int pollRes = poll(&clients[0], clients.size(), -1);

        if (pollRes < 0) {
            std::cerr << "Error: poll() failed" << std::endl;
            _exit(1);
        }

        if (pollRes > 0) {
            if (clients[0].revents & POLLIN) {
                sockaddr_in clientAddress;
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

                if (clientSocket < 0) {
                    std::cerr << "Error: accept() failed. continuing..." << std::endl;
                    continue;
                }
                pollfd clientpollfd = { clientSocket, POLLIN, 0 };
                clients.push_back(clientpollfd);
            }
        }
        for (size_t i = 1; i < clients.size(); i++) {
            if (clients[i].revents & POLLIN) {
                Request request;
                Response response;
                int clientSocket = clients[i].fd;
                request.handleRequest(clientSocket);
                response.generateResp(request);
                response.sendResp(clientSocket);
                close(clientSocket);
                clients.erase(clients.begin() + i);
                i--;
            }
        }
    }
}