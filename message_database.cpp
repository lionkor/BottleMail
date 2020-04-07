#include "message_database.h"

using scoped_lock = boost::interprocess::scoped_lock<boost::mutex>;

bottle::message_database::message_database() {
}

void bottle::message_database::put(const bottle::message& msg) {
    report("inserting " << msg.body());
    scoped_lock _lock(m_mutex);
    auto        iter = m_messages.find(msg.receiver());
    if (iter != m_messages.end()) {
        // recv exists
        iter->second.insert(iter->second.begin(), msg);
    } else {
        // recv doesn't exist
        auto [new_iter, success] = m_messages.emplace(msg.receiver(), vector<bottle::message>());
        if (success) {
            new_iter->second.insert(new_iter->second.begin(), msg);
        } else {
            ASSERT_NOT_REACHABLE();
        }
    }
}

vector<bottle::message> bottle::message_database::get(const std::string& recv) {
    try {
        return m_messages.at(recv);
    } catch (std::exception& e) {
        report("(non-fatal error) " << e.what() << " in " << __PRETTY_FUNCTION__);
        return {};
    }
}
