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

/*!
 * \brief The Receiver class, SSL server that listens on a Sender and
 *                            establishes a secure TLS connection
 * \details Receiver has the following features:
 *          1) TCP server that listens on current device IP and port 4002
 *          2) When connection is received, establishes an SSL socket
 *             and starts encryption
 *          3) If TLS handshake fails, encryption will timeout after 5s
 *             and connection will be cut
 *             If TLS handshake succeeds, device key verification occurs
 *          4) Has a state that is queryable by QML
 *          5) Uses the Messenger class to send and receive Messages
 *          6) Has an ID which is the last 1-3 digits of its IP address,
 *             which is displayed in QML and is used to allow Sender to connect
 *             to Receiver's IP
 *             Since devices will be on the same LAN, their IP address will
 *             be identical up to the last 1-3 digits
 *             This allows the Sender to enter only the last 1-3 digits instead
 *             of the entire IP address of the Receiver
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
 *    If Reciever receives an error acknowledge, it cuts the connection
 */

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
    Q_ENUM(ServerState) // Q_ENUM macro exposes this enum to qml

    /*!
     * \brief setup, sets up Receiver's IP and ID (last 1-3 digits of IP)
     * \details if internet is not connected, IP address will be a blank string
     *          and Receiver's state will become ERROR
     * \param registerDeviceList, list that holds all recognized keys
     */
    void setup(RegisterDeviceList &registerDeviceList);

protected:
    /*!
     * \brief incomingConnection, called by parent QTcpServer whenever there
     *                            is a new connection
     * \details incomingConnection is reimplemented to accept the connection
     *          and start encryption
     * \param serverSocketDescriptor, unique descriptor describing the
     *                                connected socket
     */
    void incomingConnection(qintptr serverSocketDescriptor) override;

signals:
    void connected(); // devices have connected, encrypted, keys verified
    void disconnected(); // connection has been cut
    void connectionUnrecognized(); // one or both device keys not recognized
    void connectionError(); // TLS handshake failed

    // messages received
    void receivedInfo(std::shared_ptr<InfoMessage> info);
    void receivedAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);
    void receivedText(QString text);

public slots:
    // Allows qml to query Receiver's state to check for errors
    ServerState state() const;

    bool startServer();
    void stopServer();

    // Allows qml to query Receiver's ID, the last 1-3 digits of its IP
    QString getThisID() const;

private slots:
    void socketReady(); // Socket has been encrypted
    void socketDisconnected(); // Socket has been disconnected

    // Socket is connected, but wait for encryption times out due to
    // TLS handshake failure
    void encryptingTimeout();

    // Message Handlers
    void handleReadyRead();
    void handleInfo(std::shared_ptr<InfoMessage> info);
    void handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack);

private:
    QString getIPAddress() const; // Returns device's current IP address

    /*!
     * \brief handleDeviceKey, checks if given device key is recognized
     * \details Compares deviceKey with all keys in registerDeviceList
     *          If recognized, sends Receiver's key
     *          If not, sends an error acknowledgement
     * \param deviceKey, Sender's device key
     */
    void handleDeviceKey(QString deviceKey);

    QSslSocket *serverSocket;
    ServerState serverState;

    Messenger messenger;

    QString thisID;
    QString ipAddress;
    RegisterDeviceList *registerDeviceList;

    QTimer encryptingTimer;
};

#endif // RECEIVER_HPP
