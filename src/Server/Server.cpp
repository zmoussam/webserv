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
Server::Server(std::string address, int port) {
    int reuse = 1;
    
    _port = port;
    _serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_serverSocket < 0) {
        std::cerr << "Error: socket() failed" << std::endl;
        return;
    }
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_addr.s_addr = inet_addr(address.c_str());
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
    std::vector<pollfd> clients;
    std::map<int, Request> requests;
    std::map<int, Response> responses;

    if (listen(_serverSocket, 1024) < 0) {
        std::cerr << "Error: listen() failed" << std::endl;
        return (ERROR);
    }
    pollfd serverpollfd = { _serverSocket, POLLIN, 0 };
    clients.push_back(serverpollfd);
    std::cout << "Server started on port: ::" << _port << std::endl;
    while (1337) {
        int pollRes = poll(&clients[0], clients.size(), 0);
        if (pollRes < 0) {
            std::cerr << "Error: poll() failed" << std::endl;
            return (ERROR);
        }
        if (pollRes > 0) {
            sockaddr_in clientAddress;
            if (clients[0].revents & POLLIN) {
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
                if (clientSocket < 0) {
                    std::cerr << "Error: accept() failed. continuing..." << std::endl;
                    continue;
                }
                pollfd clientpollfd = { clientSocket, POLLIN, 0 };
                clients.push_back(clientpollfd);
                std::cout << "Client connected" << std::endl;

            }
        }
        for (size_t i = 1; i < clients.size(); i++) {
            if (clients[i].revents & POLLIN) {
                std::cout << "Handling request\n";
                requests[clients[i].fd].handleRequest(clients[i].fd);
                responses[clients[i].fd].generateResp(requests[clients[i].fd], mimeTypes);
                responses[clients[i].fd].sendResp(clients[i].fd);
                // close(clients[i].fd);
                clients.erase(clients.begin() + i);
                requests.erase(clients[i].fd);
                responses.erase(clients[i].fd);
                i--;
            }
        }
    }

}
// int Server::start(void) {
//     std::vector<pollfd> clients;
//     std::map<int, time_t> keepAliveClients;
//     std::vector<Request> requests;
//     std::vector<Response> responses;
//     if (listen(_serverSocket, 1024) < 0) {
//         std::cerr << "Error: listen() failed" << std::endl;
//         return (ERROR);
//     }
//     int flags = fcntl(_serverSocket, F_GETFL, 0);
//     if (flags == -1) {
//         std::cerr << "Error getting file flags: " << strerror(errno) << std::endl;
//         close(_serverSocket);
//         return (ERROR);
//     }
//     flags |= O_NONBLOCK;
//     if (fcntl(_serverSocket, F_SETFL, flags) == -1) {
//         std::cerr << "Error setting file flags: " << strerror(errno) << std::endl;
//         close(_serverSocket);
//         return (ERROR);
//     }
//     pollfd serverpollfd = { _serverSocket, POLLIN, 0 };
//     clients.push_back(serverpollfd);
//     std::cout << "Server started on " << "http://" << inet_ntoa(_serverAddress.sin_addr) << ":" << _port << std::endl;

//     while (true) {
//         int pollRes = poll(&clients[0], clients.size(), 10);

//         if (pollRes < 0) {
//             std::cerr << "Error: poll() failed" << std::endl;
//             return (ERROR);
//         }

//         if (pollRes > 0) {
//             if (clients[0].revents & POLLIN) {
//                 sockaddr_in clientAddress;
//                 socklen_t clientAddressLength = sizeof(clientAddress);
//                 int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

//                 int flags = fcntl(clientSocket, F_GETFL, 0);
//                 if (flags == -1) {
//                     std::cerr << "Error getting file flags: " << strerror(errno) << std::endl;
//                     close(clientSocket);
//                     continue;
//                 }
//                 flags |= O_NONBLOCK;
//                 if (fcntl(clientSocket, F_SETFL, flags) == -1) {
//                     std::cerr << "Error setting file flags: " << strerror(errno) << std::endl;
//                     close(clientSocket);
//                     continue;
//                 }
//                 if (clientSocket < 0) {
//                     std::cerr << "Error: accept() failed. continuing..." << std::endl;
//                     continue;
//                 }
//                 pollfd clientpollfd = { clientSocket, POLLIN, 0 };
//                 clients.push_back(clientpollfd);
//             }
//         }
//         for (size_t i = 1; i < clients.size(); i++) {
//             int j = i - 1;
//             if (clients[i].revents & POLLIN) {
//                 Request request;
//                 Response response;
//                 requests.push_back(Request());
//                 responses.push_back(Response());
//                 responses[j].setSocket(clients[i].fd);
//                 requests[j].handleRequest(responses[j].getSocket());
//                 responses[j].generateResp(requests[j], mimeTypes);
//                 responses[j].sendResp(responses[j].getSocket());
//                 // close(responses[j].getSocket());
//             }
//         }

//     }
// }