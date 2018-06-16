#include <QFile>
#include <QNetworkInterface>

#include "sender.hpp"
#include "modules/qml/register_device_list.hpp"
#include "modules/message/text_message.hpp"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int PORT = 4002;
const int ENCRYPTING_TIMEOUT = 5000; // ms

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Sender::Sender(QObject *parent)
    : QObject(parent)
    , clientSocket()
    , clientState(ClientState::DISCONNECTED)
    , messenger()
    , thisKey("")
    , peerIPAddress("")
    , registerDeviceList(nullptr)
    , encryptingTimer(this)
{
    connect(&clientSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&clientSocket, &QSslSocket::connected,
            this, &Sender::socketConnected);
    connect(&clientSocket, &QSslSocket::encrypted,
            this, &Sender::socketReady);
    connect(&clientSocket, &QSslSocket::disconnected,
            this, &Sender::socketDisconnected);
    connect(&clientSocket, &QSslSocket::readyRead,
            this, &Sender::handleReadyRead);
    connect(this, &Sender::receivedInfo, this, &Sender::handleInfo);
    connect(this, &Sender::receivedRequest, this, &Sender::handleRequest);
    connect(this, &Sender::receivedAcknowledge, this, &Sender::handleAcknowledge);

    connect(&encryptingTimer, &QTimer::timeout,
            this, &Sender::encryptingTimeout);
    encryptingTimer.setSingleShot(true);
}

Sender::~Sender()
{
    if (clientSocket.isOpen())
        clientSocket.abort();
}

//-----------------------------------------------------------------------------
// Connection Slots
//-----------------------------------------------------------------------------
void Sender::socketConnected()
{
    encryptingTimer.start(ENCRYPTING_TIMEOUT);

    clientState = ClientState::CONNECTED;
}

void Sender::socketReady()
{
    encryptingTimer.stop();

    messenger.setDevice(&clientSocket);
    clientState = ClientState::ENCRYPTED;
}

void Sender::socketDisconnected()
{
    if (encryptingTimer.isActive())
        encryptingTimer.stop();

    switch(clientState)
    {
        case ClientState::UNRECOGNIZED:
            emit connectionUnrecognized();
            break;
        case ClientState::CONNECTED:
        case ClientState::ERROR:
            emit connectionError();
            break;
        default:
            emit disconnected();
    }

    clientState = ClientState::DISCONNECTED;
}

void Sender::socketError(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::SocketError::ConnectionRefusedError)
        connectToReceiver();
}

void Sender::encryptingTimeout()
{
    clientState = ClientState::ERROR;

    clientSocket.abort();
}

//-----------------------------------------------------------------------------
// Connection Methods
//-----------------------------------------------------------------------------

void Sender::setup(QString thisKey, RegisterDeviceList &registerDeviceList)
{
    clientSocket.addCaCertificates(QStringLiteral(":certificates/rootCA.pem"));

    QList<QSslError> errorsToIgnore;
    QList<QSslCertificate> serverCert =
        QSslCertificate::fromPath(QStringLiteral(":certificates/server.pem"));
    errorsToIgnore << QSslError(QSslError::HostNameMismatch, serverCert.at(0));
    clientSocket.ignoreSslErrors(errorsToIgnore);

    this->thisKey = thisKey;
    this->registerDeviceList = &registerDeviceList;
}

void Sender::connectToReceiver()
{
    clientSocket.connectToHostEncrypted(peerIPAddress, PORT);
    clientState = ClientState::CONNECTING;
}

void Sender::disconnectFromReceiver()
{
    if (clientState == ClientState::DISCONNECTED)
        return;

    clientSocket.abort();
    clientState = ClientState::DISCONNECTED;
}

void Sender::setPeerIPAddress(QString peerID)
{
    QStringList ipStrList = getIPAddress().split(".");
    ipStrList.replace(3, peerID);
    peerIPAddress = ipStrList.join(".");
}

QString Sender::getIPAddress() const
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            address != QHostAddress(QHostAddress::LocalHost) &&
            address.toString().section(".", -1, -1 ) != "1")
            return address.toString();
    }

    return QStringLiteral("");
}

//-----------------------------------------------------------------------------
// Read/Write
//-----------------------------------------------------------------------------

void Sender::handleReadyRead()
{
    if (!messenger.readMessage())
        return;

    switch (messenger.messageType())
    {
        case Message::MessageID::INFO:
            emit receivedInfo(std::static_pointer_cast<InfoMessage>(
                              messenger.retrieveMessage()));
            break;
        case Message::MessageID::REQUEST:
            emit receivedRequest(std::static_pointer_cast<RequestMessage>(
                                 messenger.retrieveMessage()));
            break;
        case Message::MessageID::ACKNOWLEDGE:
            emit receivedAcknowledge(std::static_pointer_cast<AcknowledgeMessage>(
                                     messenger.retrieveMessage()));
            break;
        default:
            break;
    }
}

void Sender::handleInfo(std::shared_ptr<InfoMessage> info)
{
    switch (info->infoType)
    {
        case InfoMessage::InfoType::DEVICE_KEY:
        {
            handleDeviceKey(QString(info->info));
            break;
        }
        default:
            break;
    }
}

void Sender::handleDeviceKey(QString deviceKey)
{
    const int deviceItemsSize = registerDeviceList->items().size();

    for (int i = 1; i < deviceItemsSize; i++)
    {
        if (registerDeviceList->items().at(i).deviceKey != deviceKey)
            continue;

        AcknowledgeMessage ack(AcknowledgeMessage::Acknowledge::DEVICE_KEY_OK);
        messenger.sendMessage(ack);
        emit connected();
        return;
    }

    AcknowledgeMessage ack(AcknowledgeMessage::Acknowledge::ERROR);
    messenger.sendMessage(ack);
    clientState = ClientState::UNRECOGNIZED;
}

void Sender::handleRequest(std::shared_ptr<RequestMessage> request)
{
    switch (request->request)
    {
        case RequestMessage::Request::DEVICE_KEY:
        {
            InfoMessage info(InfoMessage::InfoType::DEVICE_KEY,
                             thisKey.toUtf8());
            messenger.sendMessage(info);
            break;
        }
        default:
            break;
    }
}

void Sender::handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack)
{
    switch (ack->ack)
    {
        case AcknowledgeMessage::Acknowledge::ERROR:
        {
            clientState = ClientState::UNRECOGNIZED;
            clientSocket.disconnectFromHost();
            break;
        }
        default:
            break;
    }
}

bool Sender::sendTextMessage(QString text)
{
    TextMessage textMessage(text);
    if (!messenger.sendMessage(textMessage))
        return false;

    return true;
}
