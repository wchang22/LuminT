#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include <QDataStream>
#include <QSslSocket>
#include <memory>
#include <QVector>

#include "message.hpp"

class Messenger
{
public:
    Messenger();

    void setDevice(QSslSocket *device);
    bool sendMessage(Message &message);
    bool readMessage();
    std::shared_ptr<Message> retrieveMessage();
    Message::MessageID messageType() const;


private:
    bool frame(Message &message);

    QDataStream dataStream;
    QVector<uint8_t> messageData;
};

#endif // MESSENGER_HPP
