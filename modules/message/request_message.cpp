#include "request_message.hpp"

RequestMessage::RequestMessage(Request request)
    : request(request)
{
}

RequestMessage::RequestMessage(QByteArray &requestBytes)
    : request(static_cast<Request>(requestBytes.at(0)))
{
}

RequestMessage::~RequestMessage()
{

}

Message::MessageID RequestMessage::type() const
{
    return Message::MessageID::REQUEST;
}

QByteArray RequestMessage::serialize()
{
    QByteArray requestBytes;
    requestBytes.append(static_cast<char>(this->request));

    return requestBytes;
}
