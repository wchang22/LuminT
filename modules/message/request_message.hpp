#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include "message.hpp"

class RequestMessage : public Message
{
public:

    enum class Request : uint8_t
    {
        DEVICE_ID   = 1,
        PACKET      = 2,
    };

    RequestMessage(Request request);
    RequestMessage(QVector<uint8_t> requestVector);

    ~RequestMessage();

    Message::MessageID type() const;

    QVector<uint8_t> serialize();

    Request request;
};

#endif // REQUEST_MESSAGE_HPP
