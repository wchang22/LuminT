#ifndef SENDER_HPP
#define SENDER_HPP

#include <QObject>
#include <QSslSocket>
#include <QTimer>

#include "modules/message/messenger.hpp"
#include "modules/qml/register_device_list.hpp"
#include "modules/message/request_message.hpp"
#include "modules/message/info_message.hpp"
#include "modules/message/acknowledge_message.hpp"

class Sender : public QObject
{
    Q_OBJECT

    friend class TestCommunications;

public:
    explicit Sender(QObject *parent = nullptr);
    ~Sender();

    enum class ClientState : uint8_t
    {
        ERROR,
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        ENCRYPTING,
        ENCRYPTED,
        UNRECOGNIZED,
    };

    void setup(QString thisKey, RegisterDeviceList &registerDeviceList);

signals:
    void connected();
    void disconnected();
    void connectionUnrecognized();
    void connectionError();

    void receivedInfo(std::shared_ptr<InfoMessage> info);
    void receivedRequest(std::shared_ptr<RequestMessage> request);
    void receivedAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

public slots:
    void connectToReceiver();
    void disconnectFromReceiver();

    void setPeerIPAddress(QString peerID);
    bool sendTextMessage(QString text);

private slots:
    void socketConnected();
    void socketReady();
    void socketDisconnected();
    void socketError(QAbstractSocket::SocketError error);

    void encryptingTimeout();

    void handleReadyRead();
    void handleInfo(std::shared_ptr<InfoMessage> info);
    void handleRequest(std::shared_ptr<RequestMessage> request);
    void handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

private:
    QString getIPAddress() const;
    void handleDeviceKey(QString deviceKey);

    QSslSocket clientSocket;
    ClientState clientState;

    Messenger messenger;

    QString thisKey;
    QString peerIPAddress;
    RegisterDeviceList *registerDeviceList;

    QTimer encryptingTimer;
};

#endif // SENDER_HPP
