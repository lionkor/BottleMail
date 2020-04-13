#ifndef COMMON_H
#define COMMON_H

#include <iostream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/unordered_map.hpp>
#include <boost/array.hpp>
#include <boost/container/map.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::unordered_map;
using boost::asio::ip::tcp;
using namespace boost;
using namespace boost::container;
using namespace boost::algorithm;

template<typename _T>
struct mutexed {
    std::mutex mutex;
    _T         instance;
};

#define report_error(err) std::cout << "ERROR: " << err << std::endl
#define report(msg) std::cout << "LOG: " << msg << std::endl
#define ASSERT_NOT_REACHABLE()                                      \
    report_error(__PRETTY_FUNCTION__ << ": NOT REACHABLE REACHED"); \
    assert(false)

inline std::string read_string(tcp::socket& socket) {
    static boost::array<char, 4096> buf;
    size_t                          len = socket.read_some(boost::asio::buffer(buf));
    return std::string(buf.data(), len);
}

inline void write_string(tcp::socket& socket, const std::string& str) {
    boost::asio::write(socket, boost::asio::buffer(str));
}

inline std::string prompt(const std::string& p, char delim = '\n') {
    std::cout << p;
    std::string s;
    std::getline(std::cin, s, delim);
    return s;
}

static boost::container::map<std::string, std::string> g_passwords;

inline bool identify_clientside(tcp::socket& socket) {
    write_string(socket, prompt("Username: "));
    auto password_please = read_string(socket);
    assert(password_please == "PasswordPlease");
    write_string(socket, prompt("Password: "));
    auto ok_or_ko = read_string(socket);
    if (ok_or_ko == "OK") {
        return true;
    } else if (ok_or_ko == "KO") {
        return false;
    } else {
        ASSERT_NOT_REACHABLE();
    }
}

template<class Fnc>
inline bool identify_serverside(tcp::socket& socket, const std::string& first_message, Fnc pw_fnc) {
    auto username = first_message;
    write_string(socket, "PasswordPlease");
    auto password = read_string(socket);
    if (pw_fnc(username, password)) {
        write_string(socket, "OK");
        return true;
    } else {
        write_string(socket, "KO");
        return false;
    }
}

inline std::string conversation_find_serverside_init(tcp::socket& socket) {
    auto connect_with = read_string(socket);
    return connect_with;
}

inline void conversation_find_serverside_finalize(tcp::socket& socket, bool ok) {
    if (ok) {
        write_string(socket, "OK");
    } else {
        write_string(socket, "KO");
    }
}

inline bool conversation_find_clientside(tcp::socket& socket) {
    write_string(socket, prompt("connect with: "));
    auto found = read_string(socket);
    if (found == "OK") {
        return true;
    } else if (found == "KO") {
        return false;
    } else {
        ASSERT_NOT_REACHABLE();
    }
}

#endif // COMMON_H
