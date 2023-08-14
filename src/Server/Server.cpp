# include "Server.hpp"
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
Server::Server(int port) : _requests(), _responses(), _clients()
{
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
        std::cerr << strerror(errno) << std::endl;

        return;
    }
}

Server::Server() : _requests(), _responses(), _clients()
{
    int reuse = 1;
    _port = 8000;
    _serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_serverSocket < 0) {
        std::cerr << "Error: socket() failed" << strerror(errno) << std::endl;
        return;
    }
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    _serverAddress.sin_port = htons(_port);

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
}

Server::Server(const Server& other) {
    _port = other._port;
    _serverSocket = other._serverSocket;
    _serverAddress = other._serverAddress;
    _maxFd = other._maxFd;
}

Server& Server::operator=(const Server& other) {

    _port = other._port;
    _serverSocket = other._serverSocket;
    _serverAddress = other._serverAddress;
    return *this;
}

void Server::setPort(int port) {
	_port = port;
}

int Server::getPort() const {
	return _port;
}

int Server::getSocket() const {
    return _serverSocket;
}

 unsigned int Server::getServerAddress() const {
    return _serverAddress.sin_addr.s_addr;
}

/**
 * Starts the server by listening for incoming client connections.
 * Handles client _requests in an infinite loop.
 */
int Server::start(void) {
    std::cout << _serverSocket << std::endl;
    if (listen(_serverSocket, 512) < 0) {
        std::cerr << "Error: listen() failed" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return ERROR;
    }
    std::cout << "Server started on port: " << _port << std::endl;

    return OK;
}

int Server::addToSets(fd_set& masterSet, fd_set& readSet, fd_set& writeSet, int& maxFd) {
    unused(readSet);
    unused(writeSet);
    unused(maxFd);
    for (size_t i = 0; i < _clients.size(); i++) {
        FD_SET(_clients[i], &masterSet);
    }
    return OK;
}

int Server::handleClients(fd_set& readSet, fd_set& writeSet, fd_set &masterSet) {
    if (FD_ISSET(_serverSocket, &readSet)) {
        int clientSocket = accept(_serverSocket, NULL, NULL);
        if (clientSocket < 0 || clientSocket > 1024) {
            std::cerr << "Error: accept() failed" << std::endl;
            return ERROR;
        }
        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
        _clients.push_back(clientSocket);
        _requests[clientSocket] = Request(clientSocket);
        _responses[clientSocket] = Response(clientSocket);
    }

    for (size_t i = 0; i < _clients.size(); i++) {
        int clientSocket = _clients[i];
        if (FD_ISSET(clientSocket, &readSet)) {
            int req = _requests[clientSocket].handleRequest();
            if (req == DISCONNECTED) {
                FD_CLR(clientSocket, &masterSet);
                close(clientSocket);
                _clients.erase(_clients.begin() + i);
                _responses.erase(clientSocket);
                _requests.erase(clientSocket);
                i--;
                req = 0;
            }
        }
        if (FD_ISSET(clientSocket, &writeSet) && _requests[clientSocket].isHeadersRead() && _requests[clientSocket].isBodyRead()) {
            int res = _responses[clientSocket].sendResp(_requests[clientSocket]);
            if (res == DONE) {
                FD_CLR(clientSocket, &masterSet);
                close(clientSocket);
                _clients.erase(_clients.begin() + i);
                _responses.erase(clientSocket);
                _requests.erase(clientSocket);
                i--;
                res = 0;
            }
        }
    }
    return OK;
}