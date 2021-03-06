#include "acknowledge_message.hpp"

AcknowledgeMessage::AcknowledgeMessage(Acknowledge ack)
    : ack(ack)
{
}

AcknowledgeMessage::AcknowledgeMessage(QByteArray &ackBytes)
    : ack(static_cast<Acknowledge>(ackBytes.at(0)))
{
}

AcknowledgeMessage::~AcknowledgeMessage()
{

}

Message::MessageID AcknowledgeMessage::type() const
{
    return Message::MessageID::ACKNOWLEDGE;
}

QByteArray AcknowledgeMessage::serialize()
{
    QByteArray ackBytes;
    ackBytes.append(static_cast<char>(this->ack));

    return ackBytes;
}
