#include "messenger.hpp"

const int MESSAGE_CONTENT_OFFSET = 2;
const int MESSAGE_ID_SIZE = 1;
const int MESSAGE_LENGTH_SIZE = 1;

Messenger::Messenger()
{

}

void Messenger::setDevice(QSslSocket &device)
{
    dataStream.setDevice(&device);
}

bool Messenger::frame(Message &message)
{
    messageData.clear();
    messageData = message.serialize();

    int messageSize = messageData.size();
    if (messageSize <= 0 || messageSize > 255)
        return false;

    messageData.prepend(static_cast<char>(messageSize));
    messageData.prepend(static_cast<char>(message.type()));

    return true;
}

bool Messenger::sendMessage(Message &message)
{
    if (!frame(message))
        return false;

    int bytesWritten = dataStream.writeRawData(messageData.constData(),
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
        case Message::MessageID::SEND:
            break;
        case Message::MessageID::REQUEST:
            break;
        case Message::MessageID::ACKNOWLEDGE:
            break;
        default:
            return nullptr;
    }
}

bool Messenger::readMessage()
{
    messageData.clear();

    QByteArray messageID, messageLength, messageContent;

    dataStream.startTransaction();

    if (dataStream.readRawData(messageID.data(), MESSAGE_ID_SIZE)
        != MESSAGE_ID_SIZE)
        return false;

    messageData.append(messageID);

    if (dataStream.readRawData(messageLength.data(), MESSAGE_LENGTH_SIZE)
        != MESSAGE_LENGTH_SIZE)
        return false;

    messageData.append(messageLength);

    int messageSize = static_cast<int>(messageLength.at(0));

    if (dataStream.readRawData(messageContent.data(), messageSize)
        != messageSize)
        return false;

    messageData.append(messageContent);

    if (!dataStream.commitTransaction())
        return false;

    return true;
}
