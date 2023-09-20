# include "Server.hpp"
# include "Config.hpp"

int main(int ac, char **av) {
    if (ac != 2) {
        std::cerr << "Error: wrong number of arguments" << std::endl;
        return ERROR;
    }
    std::vector<Server> servers;
    Config config(av[1]);
    parsefile(config);
    if (config._servers.size() == 0) {
        std::cerr << "Error: no servers found" << std::endl;
        return ERROR;
    }

    // iteratore over servers
    for (std::map<int , std::vector<ServerConf> >::iterator it = config._serversByPort.begin(); it != config._serversByPort.end(); it++) {
        servers.push_back(Server(it->first, it->second));
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
        if (selectRes <= 0) {
            continue;
        }
        for (size_t i = 0; i < servers.size(); i++) {
            servers[i].handleClients(readSet, writeSet, masterSet);
        }
    }
}
