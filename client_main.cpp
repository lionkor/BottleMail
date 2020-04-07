#include <iostream>
#include "common.h"
#include "message.h"

static bool prompt_yes_no(const std::string& p) {
    std::string ans;
    do {
        std::cout << p << " [Y/N] (Y) ";
        std::getline(std::cin, ans);
    } while (ans != "Y" && ans != "N" && ans != "");
    return ans == "Y" || ans == "";
}

enum action_t
{
    SEND,
    RECV,
    EXIT
};

static action_t prompt_options() {
    std::string ans;
    do {
        std::cout << "Send / Receive / Exit [S|R|E]? ";
        std::getline(std::cin, ans);
    } while (ans != "S" && ans != "R" && ans != "E");
    if (ans == "S")
        return action_t::SEND;
    else if (ans == "R")
        return action_t::RECV;
    else
        return action_t::EXIT;
}

static inline std::string prompt(const std::string& p, char delim = '\n') {
    std::cout << p;
    std::string s;
    std::getline(std::cin, s, delim);
    return s;
}

static bool read_ack(tcp::socket& socket) {
    static boost::array<char, 4096> buf;
    auto                            len = boost::asio::read(socket, boost::asio::buffer(buf));
    bottle::message                 msg = bottle::message::from_string(std::string(buf.data(), len));
    if (msg.purpose() != bottle::message::purpose_t::ACKNOWLEDGE) {
        report_error("no ack received, instead " << msg.purpose_string());
        return false;
    }
    return true;
}

static void do_action_send(tcp::socket& socket) {
    auto sender   = prompt("sender: ");
    auto receiver = prompt("receiver: ");
    auto message  = prompt("message: ", '\x04');

    bottle::message to_send(bottle::message::purpose_t::SEND_MESSAGE, message, sender, receiver);
    boost::asio::write(socket, boost::asio::buffer(to_send.to_string()));
    read_ack(socket);
    socket.close();
}

static std::size_t write_eot(tcp::socket& socket, const std::string& sender) {
    return boost::asio::write(socket, boost::asio::buffer(bottle::message::eot(sender).to_string()));
}

static void do_action_receive(tcp::socket& socket) {
    auto sender = prompt("who am i? ");
    bool done   = false;
    while (!done) {
        bottle::message to_send(bottle::message::purpose_t::GET_MESSAGE, "", sender, "");
        boost::asio::write(socket, boost::asio::buffer(to_send.to_string()));
        boost::array<char, 4096> buf;
        std::size_t              len   = socket.read_some(boost::asio::buffer(buf));
        bottle::message          reply = bottle::message::from_string(std::string(buf.begin(), len));
        if (reply.purpose() != bottle::message::purpose_t::SEND_MESSAGE) {
            std::cout << "No messages fetched, instead got: \"" << reply.body() << "\"";
        }
        std::cout << "Message fetched: " << std::endl
                  << "-> SENDER: \"" << reply.sender() << "\"" << std::endl
                  << "-> RECEIVER: \"" << reply.receiver() << "\"" << std::endl
                  << "-> BODY START (" << std::endl
                  << reply.body() << std::endl
                  << ") BODY END" << std::endl;
        done = !prompt_yes_no("Fetch more?");
    }
    write_eot(socket, sender);
    read_ack(socket);
    socket.close();
}

int main(int argc, char** argv) {
    try {
        if (false && argc != 3) {
            std::cerr << "Usage: client <host> <port>" << std::endl;
            return 1;
        }
        boost::asio::io_context io;

        tcp::resolver resolver(io);
        auto          endpoints = resolver.resolve("localhost", "1037"); //argv[1], argv[2]);

        tcp::socket socket(io);
        boost::asio::connect(socket, endpoints);

        bool exit = false;

        while (!exit) {

            action_t decision = prompt_options();

            switch (decision) {
            case action_t::SEND:
                do_action_send(socket);
                break;
            case action_t::RECV:
                do_action_receive(socket);
                break;
            case action_t::EXIT:
                report("exiting!");
                if (socket.is_open()) {
                    socket.close();
                }
                exit = true;
                continue;
            default:
                ASSERT_NOT_REACHABLE();
            }
/*
            boost::array<char, 4096>  buf;
            boost::system::error_code error;

            std::size_t len = socket.read_some(boost::asio::buffer(buf), error);

            bottle::message reply = bottle::message::from_string(std::string(buf.data(), len));
            report("got message: ");
            report("\tpurpose: \"" << reply.purpose_string() << "\"");
            report("\tbody: \"" << reply.body() << "\"");

            report("got \"" << std::string(buf.data(), len) << "\" from server!");*/
        }

    } catch (std::exception& e) {
        std::cerr << "ERR: " << e.what() << std::endl;
    }
    report("Client Shutdown");
}
