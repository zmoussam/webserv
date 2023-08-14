# pragma once

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
# include <vector>


#include "Request.hpp"
#include "Response.hpp"
#include "Utils.hpp"
#include "Macros.hpp"

class Server {
	private:
		int _port;
		int _serverSocket;
		fd_set _masterSet;
		int _maxFd;
		struct sockaddr_in _serverAddress;
		std::map<int, Request> _requests;
    	std::map<int, Response> _responses;
		std::vector<int> _clients;
	public:
		Server();
		Server(int port);
		~Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		void setPort(int port);

		const std::string& getAddress() const;
		int getPort() const;
		int getSocket() const;
		 unsigned int getServerAddress() const;
		int start(void);
		int stop(void);
		int handleClients(fd_set& readSet, fd_set& writeSet, fd_set& masterSet);
		int addToSets(fd_set& masterSet);
};
