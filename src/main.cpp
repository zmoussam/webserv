# include "Server.hpp"

int main(void) {
    Server server(8000);
    int err = server.start();
    std::cout << "err: " << err << std::endl;
}