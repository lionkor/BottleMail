#include <iostream>
#include "common.h"
#include "message.h"
#include "message_database.h"

namespace bottle {

class tcp_connection
    : public boost::enable_shared_from_this<bottle::tcp_connection>
{
private:
    tcp::socket              m_socket;
    boost::array<char, 4096> m_buffer;
    bottle::message_database m_db;

    tcp_connection(asio::io_context& io)
        : m_socket(io) {
    }

    void send_ack() {
        bottle::message ack(bottle::message::ACKNOWLEDGE, "", "", "");
        boost::asio::write(m_socket, boost::asio::buffer(ack.to_string()));
    }

    void handle_get_message(bottle::message& initial_request) {
        assert(initial_request.purpose() == bottle::message::GET_MESSAGE);

        auto        vector = m_db.get(initial_request.sender());
        std::size_t index  = 0;

        array<char, 4096> buf;
        for (;;) {
            bottle::message request(bottle::message::EOT, "", "", "");
            auto            len = m_socket.read_some(boost::asio::buffer(buf));
            request             = bottle::message::from_string(std::string(buf.data(), len));
            if (request.purpose() == bottle::message::EOT) {
                report("EOT");
                break;
            } else if (request.purpose() == bottle::message::GET_MESSAGE) {
                report("GET_MESSAGE!");
                if (index >= vector.size()) {
                    bottle::message err_msg(bottle::message::EOT, "no more messages found (start a new query to get all messages again)", "", "");
                    boost::asio::write(m_socket, boost::asio::buffer(err_msg.to_string()));
                } else {
                    bottle::message response = vector.at(index);
                    boost::asio::write(m_socket, boost::asio::buffer(response.to_string()));
                }
                ++index;
            } else {
                ASSERT_NOT_REACHABLE();
            }
        }
        send_ack();
    }

    void handle_request(bottle::message& request) {
        switch (request.purpose()) {
        case bottle::message::SEND_MESSAGE:
            // new message to store!
            m_db.put(request);
            send_ack();
            break;
        case bottle::message::GET_MESSAGE:
            handle_get_message(request);
            break;
        case bottle::message::ACKNOWLEDGE:
            break;
        case bottle::message::EOT:
            break;
        default:
            ASSERT_NOT_REACHABLE();
        }
    }

    void handle_read(boost::system::error_code /*error*/, size_t bytes_transferred) {
        report("received " << bytes_transferred << " bytes from " << m_socket.native_handle() << ": ");
        report("_" << std::string(m_buffer.data(), bytes_transferred) << "_");

        bottle::message request = bottle::message::from_string(std::string(m_buffer.data(), bytes_transferred));

        try {
            handle_request(request);
        } catch (std::exception& e) {
            report_error(e.what() << " somewhere in handle_request");
        }
    }

public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(asio::io_context& io) {
        return pointer(new tcp_connection(io));
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
