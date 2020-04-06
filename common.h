#ifndef COMMON_H
#define COMMON_H

#include <thread>
#include <atomic>
#include <mutex>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/container/deque.hpp>
#include <boost/algorithm/string.hpp>

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

namespace bottle {



}

#endif // COMMON_H
