#include "request_message.hpp"

RequestMessage::RequestMessage(Request request)
    : request(request)
{
}

RequestMessage::RequestMessage(RequestMessage::Request request,
                               QByteArray requestInfo)
    : request(request)
    , requestInfo(requestInfo)
{
}

RequestMessage::RequestMessage(QByteArray &requestBytes)
    : request(static_cast<Request>(requestBytes.at(0)))
    , requestInfo(requestBytes.mid(1))
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
    requestBytes.append(requestInfo);

    return requestBytes;
}
