#include <iostream>
#include "common.h"

namespace bottle {

class tcp_server {
    
};

}

int main(int, char**) {
    try {
        boost::asio::io_context io_context;
        bottle::tcp_server server(io_context);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    report("Server Shutdown");
}
