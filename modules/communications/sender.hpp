#ifndef SENDER_HPP
#define SENDER_HPP

#include <QObject>
#include <QSslSocket>

#include "modules/message/messenger.hpp"
#include "modules/message/request_message.hpp"
#include "modules/message/info_message.hpp"

class Sender : public QObject
{
    Q_OBJECT
public:
    explicit Sender(QObject *parent = nullptr);
    ~Sender();

    enum class ClientState : uint8_t
    {
        DISCONNECTED        = 0,
        CONNECTING          = 1,
        ENCRYPTED           = 2,
    };


signals:
    void connected();
    void disconnected();

    void receivedRequest(std::shared_ptr<RequestMessage> request);

public slots:
    void connectToReceiver();
    void disconnectFromReceiver();
    void setup();
    void setThisID(QString thisID);

private slots:
    void ready();
    void stopped();
    void error(QAbstractSocket::SocketError error);
    void handleReadyRead();
    void handleRequest(std::shared_ptr<RequestMessage> request);

private:
    QString getIPAddress() const;

    QSslSocket clientSocket;
    ClientState clientState;

    Messenger messenger;

    QString thisID;
};

#endif // SENDER_HPP
