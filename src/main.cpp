# include "Server.hpp"

int main(void) {
    std::vector<Server> servers;
    
    for (int i = 0; i < 3; i++) {
        servers.push_back(Server(8000 + i));
    }

    for (size_t i = 0; i < servers.size(); i++) {
        servers[i].start();
    }
    fd_set masterSet, readSet, writeSet;
    FD_ZERO(&masterSet);
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    for (size_t i = 0; i < servers.size(); i++) {
        FD_SET(servers[i].getSocket(), &masterSet);
    }
    

    while (true) {
        for (size_t i = 0; i < servers.size(); i++) {
            servers[i].addToSets(masterSet);
        }

        readSet = masterSet;
        writeSet = masterSet;
        int selectRes = select(FD_SETSIZE, &readSet, &writeSet, NULL, NULL);
        if (selectRes == 0) {
            continue;
        }
        if (selectRes < 0) {
            std::cerr << "Error: select() failed" << std::endl;
            return ERROR;
        }
        for (size_t i = 0; i < servers.size(); i++) {
            servers[i].handleClients(readSet, writeSet, masterSet);
        }
    }
}
/*
localhsot   google
8080        8080 
8000 
8001 8001
*/