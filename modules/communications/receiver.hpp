#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <QTcpServer>
#include <QSslSocket>

#include "modules/message/messenger.hpp"
#include "modules/message/info_message.hpp"
#include "modules/message/acknowledge_message.hpp"
#include "modules/qml/register_device_list.hpp"

class Receiver : public QTcpServer
{
    Q_OBJECT
public:
    Receiver(QObject *parent = nullptr);
    ~Receiver();

    enum class ServerState : uint8_t
    {
        DISCONNECTED        = 0,
        CONNECTING          = 1,
        CONNECTED           = 2,
        ENCRYPTING          = 3,
        ENCRYPTED           = 4,
        RECONNECTING        = 5,
    };

    void setup(QString thisKey, RegisterDeviceList &registerDeviceList);

protected:
    void incomingConnection(qintptr serverSocketDescriptor) override;

signals:
    void connected();
    void disconnected();
    void receivedInfo(std::shared_ptr<InfoMessage> info);
    void receivedAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

public slots:
     bool startServer();
     void stopServer();

     void setPeerIPAddress(QString peerID);

private slots:
     void socketReady();
     void socketDisconnected();
     void handleReadyRead();
     void handleInfo(std::shared_ptr<InfoMessage> info);
     void handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

private:
    void handleDeviceKey(QString deviceKey);
    QString getIPAddress() const;

    QSslSocket *serverSocket;
    ServerState serverState;

    Messenger messenger;

    QString thisKey;
    QString peerIPAddress;
    RegisterDeviceList *registerDeviceList;
};

#endif // RECEIVER_HPP
