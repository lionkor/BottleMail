#ifndef MESSAGE_H
#define MESSAGE_H

#include "common.h"

namespace bottle {

class message
    : public boost::enable_shared_from_this<bottle::message>
{
public:
    enum purpose_t
    {
        GET_MESSAGE,
        SEND_MESSAGE,
        ACKNOWLEDGE,
        EOT
    };

private:
    purpose_t   m_purpose;
    std::string m_body;
    std::string m_sender;
    std::string m_receiver;

    message() = default;

public:
    message(purpose_t      purpose,
        const std::string& body,
        const std::string& sender,
        const std::string& receiver)
        : m_purpose(purpose)
        , m_body(body)
        , m_sender(sender)
        , m_receiver(receiver) {
    }

    message(const message& msg) = default;

    std::string                to_string() const;
    std::string                body() const;
    std::string                sender() const;
    std::string                receiver() const;
    std::string                purpose_string() const;
    bottle::message::purpose_t purpose() const;

    static message from_string(const std::string& str);
    static message eot(const std::string& sender);

    template<class Archive>
    void serialize(Archive& ar, const unsigned int /* version */) {
        ar& m_purpose;
        ar& m_body;
        ar& m_sender;
        ar& m_receiver;
    }
};

}

#endif // MESSAGE_H
