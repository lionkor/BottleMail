#ifndef MESSAGE_DATABASE_H
#define MESSAGE_DATABASE_H

#include "common.h"
#include "message.h"

namespace bottle {

class message_database
{
public:
    boost::mutex                              m_mutex;
    map<std::string, vector<bottle::message>> m_messages;

public:
    message_database();

    void put(const bottle::message& msg);
    vector<bottle::message> get(const std::string& recv);
};

}

#endif // MESSAGE_DATABASE_H
