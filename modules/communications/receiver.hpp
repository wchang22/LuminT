#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <QTcpServer>
#include <QSslSocket>

#include "modules/message/messenger.hpp"
#include "modules/message/info_message.hpp"
#include "modules/qml/register_device_list.hpp"

class Receiver : public QTcpServer
{
    Q_OBJECT
public:
    Receiver();
    ~Receiver();

    enum class ServerState : uint8_t
    {
        DISCONNECTED        = 0,
        CONNECTING          = 1,
        ENCRYPTING          = 2,
        ENCRYPTED           = 3,
    };

    void setup(RegisterDeviceList &registerDeviceList);

signals:
    void connected();
    void disconnected();

    void receivedInfo(std::shared_ptr<InfoMessage> info);

public slots:
     void startServer();
     void stopServer();

private slots:
     void incomingConnection(qintptr socketDescriptor);
     void connectionReceived();
     void ready();
     void stopped();
     void handleReadyRead();
     void handleInfo(std::shared_ptr<InfoMessage> info);

private:
    void handleDeviceID(QString deviceID);

    QTcpServer server;
    QSslSocket *serverSocket;
    ServerState serverState;

    RegisterDeviceList *registerDeviceList;

    Messenger messenger;
};

#endif // RECEIVER_HPP
