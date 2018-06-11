#include "messenger.hpp"
#include "request_message.hpp"
#include "info_message.hpp"
#include "acknowledge_message.hpp"
#include "text_message.hpp"

const int BYTE = 8;
const int MESSAGE_ID_BYTES = 1;
const int MESSAGE_SIZE_BYTES = 2;
const int MESSAGE_CONTENT_OFFSET = MESSAGE_ID_BYTES + MESSAGE_SIZE_BYTES;
const int MESSAGE_SIZE_INT = pow(2, MESSAGE_SIZE_BYTES * BYTE);

void Messenger::setDevice(QSslSocket *device)
{
    dataStream.setDevice(device);
}

bool Messenger::frame(Message &message)
{
    messageData.clear();
    messageData.append(message.serialize());

    int messageSize = messageData.size();
    if (messageSize <= 0 || messageSize > MESSAGE_SIZE_INT - 1)
        return false;

    for (int i = 0; i < MESSAGE_SIZE_BYTES; i++)
        messageData.prepend(static_cast<char>(
                           (messageSize >> (i * BYTE)) & 0xFF));

    messageData.prepend(static_cast<char>(message.type()));

    return true;
}

bool Messenger::sendMessage(Message &message)
{
    if (!frame(message))
        return false;

    const int bytesWritten = dataStream.writeRawData(
                                reinterpret_cast<const char*>(
                                messageData.constData()),
                                messageData.size());

    if (bytesWritten != messageData.size())
        return false;

    return true;
}

std::shared_ptr<Message> Messenger::retrieveMessage()
{
    QByteArray message(messageData.mid(MESSAGE_CONTENT_OFFSET));

    switch (static_cast<Message::MessageID>(messageData.at(0)))
    {
        case Message::MessageID::INFO:
            return std::make_shared<InfoMessage>(message);
        case Message::MessageID::REQUEST:
            return std::make_shared<RequestMessage>(message);
        case Message::MessageID::ACKNOWLEDGE:
            return std::make_shared<AcknowledgeMessage>(message);
        case Message::MessageID::TEXT:
            return std::make_shared<TextMessage>(message);
        default:
            return nullptr;
    }
}

bool Messenger::readMessage()
{
    messageData.clear();

    QByteArray messageID, messageSize, messageContent;

    dataStream.startTransaction();

    messageID.resize(MESSAGE_ID_BYTES);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageID.data()),
                               MESSAGE_ID_BYTES) != MESSAGE_ID_BYTES)
        return false;

    messageData.append(messageID);

    messageSize.resize(MESSAGE_SIZE_BYTES);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageSize.data()),
                               MESSAGE_SIZE_BYTES) != MESSAGE_SIZE_BYTES)
        return false;

    messageData.append(messageSize);

    int messageContentSize = 0;

    for (int i = 0; i < MESSAGE_SIZE_BYTES; i++)
         messageContentSize += (messageSize.at(i) <<
                               ((MESSAGE_SIZE_BYTES  - i - 1) * BYTE));

    messageContent.resize(messageContentSize);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageContent.data()),
                               messageContentSize) != messageContentSize)
        return false;

    messageData.append(messageContent);

    if (!dataStream.commitTransaction())
        return false;

    return true;
}

Message::MessageID Messenger::messageType() const
{
    if (messageData.size() < MESSAGE_CONTENT_OFFSET)
        return Message::MessageID::INVALID;

    return static_cast<Message::MessageID>(messageData.at(0));
}
