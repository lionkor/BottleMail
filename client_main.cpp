#include <iostream>
#include "common.h"

int main(int, char**) {
    try {
        boost::asio::io_context io_context;
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    report("Client Shutdown");
}
