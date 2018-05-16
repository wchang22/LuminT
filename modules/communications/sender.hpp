#ifndef SENDER_HPP
#define SENDER_HPP

#include <QObject>
#include <QSslSocket>

#include "modules/message/messenger.hpp"
#include "modules/message/request_message.hpp"
#include "modules/message/info_message.hpp"
#include "modules/message/acknowledge_message.hpp"

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

    void setup(QString thisID);

signals:
    void connected();
    void disconnected();

    void receivedRequest(std::shared_ptr<RequestMessage> request);
    void receivedAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

public slots:
    void connectToReceiver();
    void disconnectFromReceiver();

private slots:
    void ready();
    void stopped();
    void error(QAbstractSocket::SocketError error);
    void handleReadyRead();
    void handleRequest(std::shared_ptr<RequestMessage> request);
    void handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

private:
    QString getIPAddress() const;

    QSslSocket clientSocket;
    ClientState clientState;

    Messenger messenger;

    QString thisID;
};

#endif // SENDER_HPP
