#ifndef ACKNOWLEDGE_MESSAGE_HPP
#define ACKNOWLEDGE_MESSAGE_HPP

#include "message.hpp"

/*!
 * \brief The AcknowledgeMessage class, acknowledges info has been received
 */

class AcknowledgeMessage : public Message
{
public:

    enum class Acknowledge : char
    {
        ERROR,          // General error, often device key is invalid
        DEVICE_KEY_OK,  // Device key is accepted
        ENUM_END,       // Marks end of num, used for testing
    };

    AcknowledgeMessage(Acknowledge ack);
    AcknowledgeMessage(QByteArray &ackBytes);

    ~AcknowledgeMessage();

    Message::MessageID type() const;

    QByteArray serialize();

    Acknowledge ack;
};

#endif // ACKNOWLEDGE_MESSAGE_HPP
