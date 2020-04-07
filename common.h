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
#define ASSERT_NOT_REACHABLE() report_error(__PRETTY_FUNCTION__ << ": NOT REACHABLE REACHED"); assert(false)

#endif // COMMON_H
