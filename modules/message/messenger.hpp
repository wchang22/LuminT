#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include <QDataStream>
#include <QSslSocket>
#include <memory>

#include "message.hpp"

class Messenger
{
    friend class TestMessage;

public:
    void setDevice(QSslSocket *device);
    bool sendMessage(Message &message);
    bool readMessage();

    std::shared_ptr<Message> retrieveMessage();
    Message::MessageID messageType() const;

private:
    bool frame(Message &message);

    QDataStream dataStream;
    QByteArray messageData;
};

#endif // MESSENGER_HPP
