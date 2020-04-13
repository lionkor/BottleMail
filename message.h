#ifndef MESSAGE_H
#define MESSAGE_H

#include "common.h"

namespace bottle {

class message
    : public boost::enable_shared_from_this<bottle::message>
{
public:
    enum type
    {
        PUT_MSG,
        GET_MSG,
    };

private:
    message::type m_type;
    std::string   m_body;
    std::string   m_sender;
    std::string   m_receiver;

    message() = default;

public:
    message(message::type  type,
        const std::string& body,
        const std::string& sender,
        const std::string& receiver)
        : m_type(type)
        , m_body(body)
        , m_sender(sender)
        , m_receiver(receiver) {
    }

    message(const message& msg) = default;

    std::string   to_string() const;
    std::string   body() const;
    std::string   sender() const;
    std::string   receiver() const;
    message::type type() const;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int /* version */) {
        ar& m_type;
        ar& m_body;
        ar& m_sender;
        ar& m_receiver;
    }
};

}

#endif // MESSAGE_H
