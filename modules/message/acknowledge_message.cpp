#include "acknowledge_message.hpp"

AcknowledgeMessage::AcknowledgeMessage(Acknowledge ack)
{
    this->ack = ack;
}

AcknowledgeMessage::AcknowledgeMessage(QVector<uint8_t> ackVector)
{
    this->ack = static_cast<Acknowledge>(ackVector.front());
}

AcknowledgeMessage::~AcknowledgeMessage()
{

}

Message::MessageID AcknowledgeMessage::type() const
{
    return Message::MessageID::ACKNOWLEDGE;
}

QVector<uint8_t> AcknowledgeMessage::serialize()
{
    QVector<uint8_t> ackVector;
    ackVector.append(static_cast<uint8_t>(this->ack));

    return ackVector;
}
