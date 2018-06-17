#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <QTcpServer>
#include <QSslSocket>
#include <QTimer>

#include "modules/message/messenger.hpp"
#include "modules/message/info_message.hpp"
#include "modules/message/acknowledge_message.hpp"
#include "modules/message/text_message.hpp"
#include "modules/qml/register_device_list.hpp"

class Receiver : public QTcpServer
{
    Q_OBJECT

    friend class TestCommunications;

public:
    Receiver(QObject *parent = nullptr);
    ~Receiver();

    enum class ServerState : uint8_t
    {
        ERROR,
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        ENCRYPTING,
        ENCRYPTED,
        UNRECOGNIZED,
    };
    Q_ENUM(ServerState)

    void setup(RegisterDeviceList &registerDeviceList);

protected:
    void incomingConnection(qintptr serverSocketDescriptor) override;

signals:
    void connected();
    void disconnected();
    void connectionUnrecognized();
    void connectionError();

    void receivedInfo(std::shared_ptr<InfoMessage> info);
    void receivedAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);
    void receivedText(QString text);

public slots:
    ServerState state() const;

    bool startServer();
    void stopServer();

    QString getThisID() const;

private slots:
    void socketReady();
    void socketDisconnected();

    void encryptingTimeout();

    void handleReadyRead();
    void handleInfo(std::shared_ptr<InfoMessage> info);
    void handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

private:
    void handleDeviceKey(QString deviceKey);
    QString getIPAddress() const;

    QSslSocket *serverSocket;
    ServerState serverState;

    Messenger messenger;

    QString thisID;
    QString ipAddress;
    RegisterDeviceList *registerDeviceList;

    QTimer encryptingTimer;
};

#endif // RECEIVER_HPP
