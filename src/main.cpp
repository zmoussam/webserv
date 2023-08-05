# include "Server.hpp"

int main(void) {
    Server server("127.0.0.1", 8000);
    server.start();
}