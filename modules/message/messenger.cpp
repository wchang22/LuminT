#include "messenger.hpp"
#include "request_message.hpp"
#include "info_message.hpp"
#include "acknowledge_message.hpp"
#include "text_message.hpp"

namespace MessageSize
{
    const int BYTE = 8;
    const int MESSAGE_ID_BYTES = 1;
    const int MESSAGE_SIZE_BYTES = 2;
    const int MESSAGE_CONTENT_OFFSET = MESSAGE_ID_BYTES + MESSAGE_SIZE_BYTES;
    const int MESSAGE_SIZE_INT = pow(2, MESSAGE_SIZE_BYTES * BYTE);
}

void Messenger::setDevice(QSslSocket *device)
{
    dataStream.setDevice(device);
}

bool Messenger::frame(Message &message)
{
    messageData.clear();
    messageData.append(message.serialize());

    int messageSize = messageData.size();
    if (messageSize <= 0 || messageSize > MessageSize::MESSAGE_SIZE_INT - 1)
        return false;

    for (int i = 0; i < MessageSize::MESSAGE_SIZE_BYTES; i++)
        messageData.prepend(static_cast<char>(
                           (messageSize >> (i * MessageSize::BYTE)) & 0xFF));

    messageData.prepend(static_cast<char>(message.type()));

    return true;
}

bool Messenger::frame(FileMessage &message)
{
    messageData.clear();
    messageData.append(message.serialize());

    int messageSize = messageData.size();
    if (messageSize <= 0 || messageSize > FileSize::PACKET_BYTES - 1)
        return false;

    for (int i = 0; i < FileSize::SEQ_BYTES; i++)
        messageData.prepend(static_cast<char>(
                           (message.seq >> (i * FileSize::BYTE)) & 0xFF));

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

bool Messenger::sendMessage(FileMessage &message)
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
    QByteArray message(messageData.mid(MessageSize::MESSAGE_CONTENT_OFFSET));

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

std::shared_ptr<FileMessage> Messenger::retrieveFile()
{
    return std::make_shared<FileMessage>(messageData);
}

bool Messenger::readMessage()
{
    messageData.clear();

    QByteArray messageID, messageSize, messageContent;

    dataStream.startTransaction();

    messageID.resize(MessageSize::MESSAGE_ID_BYTES);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageID.data()),
                               MessageSize::MESSAGE_ID_BYTES) !=
                               MessageSize::MESSAGE_ID_BYTES)
        return false;

    messageData.append(messageID);

    messageSize.resize(MessageSize::MESSAGE_SIZE_BYTES);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageSize.data()),
                               MessageSize::MESSAGE_SIZE_BYTES) !=
                               MessageSize::MESSAGE_SIZE_BYTES)
        return false;

    messageData.append(messageSize);

    int messageContentSize = 0;

    for (int i = 0; i < MessageSize::MESSAGE_SIZE_BYTES; i++)
         messageContentSize += (messageSize.at(i) <<
                               ((MessageSize::MESSAGE_SIZE_BYTES  - i - 1) *
                                 MessageSize::BYTE));

    messageContent.resize(messageContentSize);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageContent.data()),
                               messageContentSize) != messageContentSize)
        return false;

    messageData.append(messageContent);

    if (!dataStream.commitTransaction())
        return false;

    return true;
}

bool Messenger::readFile(uint32_t packetSize)
{
    messageData.clear();
    dataStream.resetStatus();

    QByteArray messageSeq, messageContent;

    dataStream.startTransaction();

    messageSeq.resize(FileSize::SEQ_BYTES);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageSeq.data()),
                               FileSize::SEQ_BYTES) !=
                               FileSize::SEQ_BYTES)
    {
        dataStream.rollbackTransaction();
        return false;
    }

    if (dataStream.status() != QDataStream::Status::Ok)
    {
        dataStream.rollbackTransaction();
        return false;
    }

    messageData.append(messageSeq);

    messageContent.resize(packetSize);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageContent.data()),
                               packetSize) != packetSize)
    {
        dataStream.rollbackTransaction();
        return false;
    }

    if (dataStream.status() != QDataStream::Status::Ok)
    {
        dataStream.rollbackTransaction();
        return false;
    }

    messageData.append(messageContent);

    dataStream.commitTransaction();

    return true;
}

Message::MessageID Messenger::messageType() const
{
    if (messageData.size() < MessageSize::MESSAGE_CONTENT_OFFSET)
        return Message::MessageID::INVALID;

    return static_cast<Message::MessageID>(messageData.at(0));
}
