#ifndef ACKNOWLEDGE_MESSAGE_HPP
#define ACKNOWLEDGE_MESSAGE_HPP

#include "message.hpp"

class AcknowledgeMessage : public Message
{
public:

    enum class Acknowledge : uint8_t
    {
        ERROR               = 0,
        DEVICE_ID_OK        = 1,
        DEVICE_ID_INVALID   = 2,
    };

    AcknowledgeMessage(Acknowledge ack);
    AcknowledgeMessage(QVector<uint8_t> ackVector);

    ~AcknowledgeMessage();

    Message::MessageID type() const;

    QVector<uint8_t> serialize();

    Acknowledge ack;
};

#endif // ACKNOWLEDGE_MESSAGE_HPP
