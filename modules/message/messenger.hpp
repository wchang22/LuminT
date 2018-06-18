#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include <QDataStream>
#include <QSslSocket>
#include <memory>

#include "message.hpp"
#include "file_message.hpp"

class Messenger
{
    friend class TestMessage;

public:
    void setDevice(QSslSocket *device);
    bool sendMessage(Message &message);
    bool sendMessage(FileMessage &message);
    bool readMessage();
    bool readFile(uint32_t packetSize);

    std::shared_ptr<Message> retrieveMessage();
    std::shared_ptr<FileMessage> retrieveFile();
    Message::MessageID messageType() const;

private:
    bool frame(Message &message);
    bool frame(FileMessage &message);

    QDataStream dataStream;
    QByteArray messageData;
};

#endif // MESSENGER_HPP
