#include "request_message.hpp"

RequestMessage::RequestMessage(Request request)
{
    this->request = request;
}

RequestMessage::RequestMessage(QVector<uint8_t> &requestVector)
{
    this->request = static_cast<Request>(requestVector.front());
}

RequestMessage::~RequestMessage()
{

}

Message::MessageID RequestMessage::type() const
{
    return Message::MessageID::REQUEST;
}

QVector<uint8_t> RequestMessage::serialize()
{
    QVector<uint8_t> requestVector;
    requestVector.append(static_cast<uint8_t>(this->request));

    return requestVector;
}
