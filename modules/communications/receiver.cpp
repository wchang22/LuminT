#include <QSslKey>
#include <QFile>
#include <QNetworkInterface>

#include "receiver.hpp"
#include "modules/message/request_message.hpp"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int PORT = 4002;

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Receiver::Receiver(QObject *parent)
    : QTcpServer(parent)
    , serverSocket(nullptr)
    , serverState(ServerState::DISCONNECTED)
    , messenger()
    , thisKey("")
    , peerIPAddress("")
    , registerDeviceList(nullptr)
{
    connect(this, &Receiver::receivedInfo,
            this, &Receiver::handleInfo);
    connect(this, &Receiver::receivedAcknowledge,
            this, &Receiver::handleAcknowledge);
}

Receiver::~Receiver()
{
    if (this->isListening())
        this->close();

    if (serverSocket && serverSocket->isOpen())
        serverSocket->abort();
}

//-----------------------------------------------------------------------------
// Connection Slots
//-----------------------------------------------------------------------------

void Receiver::incomingConnection(qintptr serverSocketDescriptor)
{
    this->close();

    serverSocket = new QSslSocket(this);

    if (!serverSocket->setSocketDescriptor(serverSocketDescriptor))
        return;

    serverState = ServerState::CONNECTED;

    connect(serverSocket, &QSslSocket::encrypted,
            this, &Receiver::socketReady);
    connect(serverSocket, &QSslSocket::readyRead,
            this, &Receiver::handleReadyRead);
    connect(serverSocket, &QSslSocket::disconnected,
            this, &Receiver::socketDisconnected);

    addPendingConnection(serverSocket);

    serverState = ServerState::ENCRYPTING;

    QFile certFile(QStringLiteral(":certificates/server.pem"));
    QFile keyFile(QStringLiteral(":certificates/server.key"));
    if (!certFile.open(QFile::ReadOnly) || !keyFile.open(QFile::ReadOnly))
        return;

    QSslCertificate cert = QSslCertificate(certFile.readAll());
    QSslKey key = QSslKey(keyFile.readAll(), QSsl::Rsa, QSsl::Pem);

    certFile.close();
    keyFile.close();

    if (key.isNull())
        return;

    serverSocket->setLocalCertificate(cert);
    serverSocket->setPrivateKey(key);

    serverSocket->startServerEncryption();
}

void Receiver::socketReady()
{
    messenger.setDevice(serverSocket);

    RequestMessage requestKey(RequestMessage::Request::DEVICE_KEY);
    messenger.sendMessage(requestKey);

    serverState = ServerState::ENCRYPTED;
}

void Receiver::socketDisconnected()
{
    if (serverState == ServerState::ERROR)
        emit connectionError();

    stopServer();
}

//-----------------------------------------------------------------------------
// Connection Methods
//-----------------------------------------------------------------------------

void Receiver::setup(QString thisKey, RegisterDeviceList &registerDeviceList)
{
    this->thisKey = thisKey;
    this->registerDeviceList = &registerDeviceList;
}

bool Receiver::startServer()
{
    if (serverState == ServerState::CONNECTING)
        this->close();

    if (!this->listen(QHostAddress(peerIPAddress), PORT))
        return false;

    serverState = ServerState::CONNECTING;
    return true;
}

void Receiver::stopServer()
{
    if (serverState == ServerState::DISCONNECTED)
        return;

    if (serverState != ServerState::ERROR)
        emit disconnected();

    if (this->isListening())
        this->close();

    if (serverState == ServerState::CONNECTING)
    {
        serverState = ServerState::DISCONNECTED;
        return;
    }

    disconnect(serverSocket, &QSslSocket::encrypted,
            this, &Receiver::socketReady);
    disconnect(serverSocket, &QSslSocket::readyRead,
            this, &Receiver::handleReadyRead);
    disconnect(serverSocket, &QSslSocket::disconnected,
            this, &Receiver::socketDisconnected);

    serverSocket->abort();

    serverState = ServerState::DISCONNECTED;
}

void Receiver::setPeerIPAddress(QString peerID)
{
    QString ip(getIPAddress());
    QStringList ipStrList = ip.split(".");
    ipStrList.replace(3, peerID);
    peerIPAddress = ipStrList.join(".");
}

QString Receiver::getIPAddress() const
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            address != QHostAddress(QHostAddress::LocalHost))
            return address.toString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Read/Write
//-----------------------------------------------------------------------------

void Receiver::handleReadyRead()
{
    if (!messenger.readMessage())
        return;

    switch (messenger.messageType())
    {
        case Message::MessageID::INFO:
            emit receivedInfo(std::static_pointer_cast<InfoMessage>(
                              messenger.retrieveMessage()));
            break;
        case Message::MessageID::ACKNOWLEDGE:
            emit receivedAcknowledge(std::static_pointer_cast<AcknowledgeMessage>(
                                     messenger.retrieveMessage()));
            break;
        case Message::MessageID::TEXT:
            emit receivedText(std::static_pointer_cast<TextMessage>(
                              messenger.retrieveMessage())->text);
            break;
        default:
            break;
    }
}

void Receiver::handleInfo(std::shared_ptr<InfoMessage> info)
{
    switch (info->infoType)
    {
        case InfoMessage::InfoType::DEVICE_KEY:
        {
            handleDeviceKey(byteVectorToString(info->info));
            break;
        }
        default:
            break;
    }
}

void Receiver::handleDeviceKey(QString deviceKey)
{
    const int deviceItemsSize = registerDeviceList->items().size();

    for (int i = 1; i < deviceItemsSize; i++)
    {
        if (registerDeviceList->items().at(i).deviceKey != deviceKey)
            continue;

        InfoMessage info(InfoMessage::InfoType::DEVICE_KEY,
                         stringToByteVector(thisKey));
        messenger.sendMessage(info);
        return;
    }

    AcknowledgeMessage ack(AcknowledgeMessage::Acknowledge::ERROR);
    messenger.sendMessage(ack);

    serverState = ServerState::ERROR;
}

void Receiver::handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack)
{
    switch (ack->ack)
    {
        case AcknowledgeMessage::Acknowledge::ERROR:
        {
            serverState = ServerState::ERROR;
            serverSocket->abort();
            break;
        }
        case AcknowledgeMessage::Acknowledge::DEVICE_KEY_OK:
        {
            emit connected();
            break;
        }
        default:
            break;
    }
}
