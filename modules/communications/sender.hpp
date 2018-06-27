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

/*!
 * \brief The Sender class, SSL socket that connects to a Receiver and
 *                            establishes a secure TLS connection
 * \details Sender has the following features:
 *          1) SSL socket that connects to Receiver's IP and port 4002
 *          2) If TLS handshake fails, encryption will timeout after 5s
 *             and connection will be cut
 *             If TLS handshake succeeds, device key verification occurs
 *          3) Uses the Messenger class to send and receive Messages
 *          4) Connects to Receiver's IP via Receiver's ID, which is the
 *             last 1-3 digits of Receiver's IP
 *             Since both devices are connected to the same LAN, their IP
 *             addresses are identical for the first 3 parts
 *             Receiver will display the last part for the user to enter
 *             into Sender
 *             Sender then replaces the last part of its own IP with the
 *             ID, obtaining Receiver's IP
 *             This prevents the user from having to enter the entire IP
 */

/*
 * Device Key Verification Process:
 *
 * 1) Receiver sends a request for Sender's key
 * 2) Sender sends its key in an info message
 * 3) If Receiver has Sender's key in its list of recognized keys,
 *      Receiver sends its own key to Sender in an info message
 *    If Receiver does not, it sends an error acknowledge message
 * 4) If Sender receives Receiver's key, it checks with its list
 *      If Sender recognizes it, it sends a device key ok acknowledgement
 *          and is now ready
 *      If Sender does not recognize it, it sends an error acknowledge
 *    If Sender receives an acknowledge error message, it cuts the connection
 * 5) If Receiver receives a device key ok acknowledgement, it is now ready
 *    If Receiver receives an error acknowledge, it cuts the connection
 */

class Sender : public QObject
{
    Q_OBJECT

    friend class TestCommunications;

public:
    explicit Sender(QObject *parent = nullptr);
    ~Sender();

    enum class ClientState
    {
        ERROR,
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        ENCRYPTING,
        ENCRYPTED,
        UNRECOGNIZED,
    };

    enum class MessageState
    {
        MESSAGE,
        FILE_SENDING,
        FILE_PAUSED,
    };

    /*!
     * \brief setup, sets up the CA certificate and recognized device keys
     * \details the CA certificate is used in the TLS handshake
     *          Receiver's certificate is signed by the CA certificate, so
     *          the CA certificate is used to ensure that Receiver is trusted
     *          and that a secure connection should be established
     * \param registerDeviceList, list that holds all recognized keys
     */
    void setup(RegisterDeviceList &registerDeviceList);

signals:
    void connected(); // devices have connected, encrypted, keys verified
    void disconnected(); // connection has been cut
    void connectionUnrecognized(); // one or both device keys not recognized
    void connectionError(); // TLS handshake failed

    // messages received
    void receivedInfo(std::shared_ptr<InfoMessage> info);
    void receivedRequest(std::shared_ptr<RequestMessage> request);
    void receivedAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

    void sendProgress(float progress);
    void fileCompleted();
    void fileError();

public slots:
    void connectToReceiver();
    void disconnectFromReceiver();

    // Sets Receiver's IP address using Receiver's ID
    void setPeerIPAddress(QString peerID);
    bool sendTextMessage(QString text);
    bool sendFile(QString filePath);

private slots:
    void socketConnected(); // Socket has been connected
    void socketReady(); // Socket has been encrypted
    void socketDisconnected(); // Socket has been disconnected
    // Called whenever the socket has an error
    void socketError(QAbstractSocket::SocketError error);

    // Socket is connected, but wait for encryption times out due to
    // TLS handshake failure
    void encryptingTimeout();

    // Message Handlers
    void handleReadyRead();
    void handleInfo(std::shared_ptr<InfoMessage> info);
    void handleRequest(std::shared_ptr<RequestMessage> request);
    void handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

private:
    QString getIPAddress() const; // Returns device's current IP address

    /*!
     * \brief handleDeviceKey, checks if given device key is recognized
     * \details Compares deviceKey with all keys in registerDeviceList
     *          If recognized, sends Senders's key
     *          If not, sends an error acknowledgement
     * \param deviceKey, Receiver's device key
     */
    void handleDeviceKey(QString deviceKey);

    QSslSocket clientSocket;
    ClientState clientState;
    MessageState messageState;

    Messenger messenger;

    QString peerIPAddress;
    RegisterDeviceList *registerDeviceList;

    QTimer encryptingTimer;

    QString currentFilePath;
    qint64 currentFileSize;
};

#endif // SENDER_HPP
