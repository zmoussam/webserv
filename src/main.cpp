# include "Server.hpp"

void    f() {
    exit(EXIT_FAILURE);
}

int main(void) {
    // std::set_new_handler(f);
    Server server(8000);
    int err = server.start();
    std::cout << "err: " << err << std::endl;
}