#include <iostream>
#include "test.hpp"

int main() {
    try {
        throw HTTPException("404 Not Found");
    } catch (const HTTPException& e) {
        std::cerr << "HTTPException caught: " << e.what() << std::endl;
    }

    return 0;
}