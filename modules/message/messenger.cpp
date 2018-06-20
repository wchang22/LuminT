#include "messenger.hpp"
#include "request_message.hpp"
#include "info_message.hpp"
#include "acknowledge_message.hpp"
#include "text_message.hpp"
#include "modules/utilities/utilities.hpp"

void Messenger::setDevice(QSslSocket *device)
{
    dataStream.setDevice(device);
}

bool Messenger::frame(Message &message)
{
    messageData.clear();
    messageData.append(message.serialize());

    int messageSize = messageData.size();
    if (messageSize <= 0 || messageSize > LuminT::MESSAGE_SIZE_INT - 1)
        return false;

    for (int i = 0; i < LuminT::MESSAGE_SIZE_BYTES; i++)
        messageData.prepend(static_cast<char>(
                           (messageSize >> (i * LuminT::BYTE)) & 0xFF));

    messageData.prepend(static_cast<char>(message.type()));

    return true;
}

bool Messenger::frame(FileMessage &message)
{
    messageData.clear();
    messageData.append(message.serialize());

    int messageSize = messageData.size();
    if (messageSize <= 0 || messageSize > LuminT::PACKET_BYTES)
        return false;

    for (int i = 0; i < LuminT::SEQ_BYTES; i++)
        messageData.prepend(static_cast<char>(
                           (message.seq >> (i * LuminT::BYTE)) & 0xFF));

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
    QByteArray message(messageData.mid(LuminT::MESSAGE_CONTENT_OFFSET));

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

    messageID.resize(LuminT::MESSAGE_ID_BYTES);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageID.data()),
                               LuminT::MESSAGE_ID_BYTES) !=
                               LuminT::MESSAGE_ID_BYTES)
        return false;

    messageData.append(messageID);

    messageSize.resize(LuminT::MESSAGE_SIZE_BYTES);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageSize.data()),
                               LuminT::MESSAGE_SIZE_BYTES) !=
                               LuminT::MESSAGE_SIZE_BYTES)
        return false;

    messageData.append(messageSize);

    int messageContentSize = 0;

    for (int i = 0; i < LuminT::MESSAGE_SIZE_BYTES; i++)
         messageContentSize += (static_cast<uint8_t>(messageSize.at(i)) <<
                               ((LuminT::MESSAGE_SIZE_BYTES  - i - 1) *
                                 LuminT::BYTE));

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

    if (dataStream.status() != QDataStream::Status::Ok)
    {
        dataStream.rollbackTransaction();
        return false;
    }

    messageSeq.resize(LuminT::SEQ_BYTES);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageSeq.data()),
                               LuminT::SEQ_BYTES) < LuminT::SEQ_BYTES)
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
                               packetSize) < packetSize)
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
    if (messageData.size() < LuminT::MESSAGE_CONTENT_OFFSET)
        return Message::MessageID::INVALID;

    return static_cast<Message::MessageID>(messageData.at(0));
}
