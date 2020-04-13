#include <iostream>
#include "common.h"
#include "message.h"
#include "message_database.h"

namespace bottle {

typedef boost::container::map<std::string, tcp::socket> socket_map;

class tcp_connection
    : public boost::enable_shared_from_this<bottle::tcp_connection>
{
private:
    tcp::socket              m_socket;
    boost::array<char, 4096> m_buffer;
    bottle::message_database m_db;
    mutexed<socket_map>&     m_socket_map;

    tcp_connection(asio::io_context& io, mutexed<socket_map>& socket_map)
        : m_socket(io)
        , m_socket_map(socket_map) {
    }

    void handle_read(boost::system::error_code error,
        size_t                                 len) {
        if (error) {
            report_error(error.message());
            return;
        }
        std::string first_msg(m_buffer.data(), len);
        auto        always_yes = [&](auto str1, auto str2) {
            return true;
        };
        bool ok = identify_serverside(m_socket, first_msg, always_yes);
        report(ok);
        if (!ok) {
            return;
        }
        std::string connect_with = conversation_find_serverside_init(m_socket);
        // find the person or don't
        bool was_found = true;
        conversation_find_serverside_finalize(m_socket, was_found);
    }

public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(asio::io_context& io, mutexed<socket_map>& socket_map) {
        return pointer(new tcp_connection(io, socket_map));
    }

    tcp::socket& socket() {
        return m_socket;
    }

    void start() {
        m_socket.async_read_some(boost::asio::buffer(m_buffer), boost::bind(&bottle::tcp_connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
};

class tcp_server
{
    asio::io_context& m_io;
    tcp::acceptor     m_acceptor;

public:
    tcp_server(asio::io_context& io)
        : m_io(io), m_acceptor(io, tcp::endpoint(tcp::v4(), 1037)) {
        start_accept();
    }
    void start_accept() {
        tcp_connection::pointer new_connection = tcp_connection::create(m_io);
        m_acceptor.async_accept(
            new_connection->socket(),
            boost::bind(&tcp_server::handle_accept,
                this,
                new_connection,
                boost::asio::placeholders::error()));
    }

    void handle_accept(tcp_connection::pointer new_connection,
        const boost::system::error_code&       error) {
        if (!error) {
            new_connection->start();
        }
        start_accept();
    }
};
}

int main(int, char**) {
    try {
        boost::asio::io_context io_context;
        bottle::tcp_server      server(io_context);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    report("Server Shutdown");
}
