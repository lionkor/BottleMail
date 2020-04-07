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

bottle::message::purpose_t bottle::message::purpose() const {
    return m_purpose;
}

std::string bottle::message::purpose_string() const {
    switch (m_purpose) {
    case purpose_t::GET_MESSAGE:
        return nameof(purpose_t::GET_MESSAGE);
    case purpose_t::SEND_MESSAGE:
        return nameof(purpose_t::SEND_MESSAGE);
    case purpose_t::ACKNOWLEDGE:
        return nameof(purpose_t::ACKNOWLEDGE);
    case purpose_t::EOT:
        return nameof(purpose_t::EOT);
    default:
        return "not implemented";
    }
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

bottle::message bottle::message::from_string(const std::string& str) {
    if (str.empty()) {
        throw std::runtime_error("empty string passed to " + std::string(__FUNCTION__));
    }
    std::stringstream istr(str);
    message           msg;
    {
        boost::archive::text_iarchive ia(istr);
        ia >> msg;
    }
    return msg;
}

bottle::message bottle::message::eot(const std::string& sender) {
    return bottle::message(bottle::message::purpose_t::EOT, "", sender, "");
}
