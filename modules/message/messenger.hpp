#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include <QDataStream>
#include <QByteArray>
#include <QSslSocket>
#include <memory>

#include "message.hpp"

class Messenger
{
public:
    Messenger();

    void setDevice(QSslSocket &device);
    bool sendMessage(Message &message);
    bool readMessage();


private:
    bool frame(Message &message);
    std::shared_ptr<Message> retrieveMessage();

    QDataStream dataStream;
    QByteArray messageData;
};

#endif // MESSENGER_HPP
