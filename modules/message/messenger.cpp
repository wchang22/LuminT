#include "messenger.hpp"
#include "request_message.hpp"

const int MESSAGE_ID_SIZE = 1;
const int MESSAGE_SIZE_SIZE = 1;
const int MESSAGE_CONTENT_OFFSET = MESSAGE_ID_SIZE + MESSAGE_SIZE_SIZE;


Messenger::Messenger()
{

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
    if (messageSize <= 0 || messageSize > 255)
        return false;

    messageData.prepend(static_cast<uint8_t>(messageSize));
    messageData.prepend(static_cast<uint8_t>(message.type()));

    return true;
}

bool Messenger::sendMessage(Message &message)
{
    if (!frame(message))
        return false;

    int bytesWritten = dataStream.writeRawData(reinterpret_cast<const char*>(
                                               messageData.constData()),
                                               messageData.size());

    if (bytesWritten != messageData.size())
        return false;

    return true;
}

std::shared_ptr<Message> Messenger::retrieveMessage()
{
    QVector<uint8_t> message(messageData.mid(MESSAGE_CONTENT_OFFSET));

    switch (static_cast<Message::MessageID>(messageData.front()))
    {
        case Message::MessageID::REQUEST:
            return std::make_shared<RequestMessage>(message);
        default:
            return nullptr;
    }
}

bool Messenger::readMessage()
{
    messageData.clear();

    QVector<uint8_t> messageID, messageSize, messageContent;

    dataStream.startTransaction();

    messageID.resize(MESSAGE_ID_SIZE);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageID.data()),
                               MESSAGE_ID_SIZE) != MESSAGE_ID_SIZE)
        return false;

    messageData.append(messageID);

    messageSize.resize(MESSAGE_SIZE_SIZE);
    if (dataStream.readRawData(reinterpret_cast<char*>(messageSize.data()),
                               MESSAGE_SIZE_SIZE) != MESSAGE_SIZE_SIZE)
        return false;

    messageData.append(messageSize);

    int messageContentSize = static_cast<int>(messageSize.front());

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

    return static_cast<Message::MessageID>(messageData.front());
}
