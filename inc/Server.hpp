# pragma once

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <sstream>


#include "Request.hpp"
#include "Response.hpp"
#include "Utils.hpp"
#include "Macros.hpp"

class Server {
	private:
		int _port;
		int _serverSocket;
		struct sockaddr_in _serverAddress;
	
	public:
		Server(int port);
		~Server();

		void setPort(int port);

		const std::string& getAddress() const;
		int getPort() const;

		int start();
};
