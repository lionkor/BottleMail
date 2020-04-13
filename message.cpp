#include "message.h"

#define nameof(x) #x

std::string bottle::message::to_string() const {
    std::stringstream ostr;
    {
        boost::archive::text_oarchive oa(ostr);
        oa << *this;
    }
    return ostr.str();
}

std::string bottle::message::body() const {
    return m_body;
}

std::string bottle::message::sender() const {
    return m_sender;
}

std::string bottle::message::receiver() const {
    return m_receiver;
}

