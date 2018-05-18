#include <QSslKey>
#include <QFile>

#include "receiver.hpp"
#include "modules/message/request_message.hpp"
#include "modules/message/acknowledge_message.hpp"

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
    , registerDeviceList(nullptr)
    , messenger()
{
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
    serverState = ServerState::ENCRYPTING;

    this->close();

    serverSocket = new QSslSocket(this);

    if (!serverSocket->setSocketDescriptor(serverSocketDescriptor))
        return;

    QFile certFile(QStringLiteral("server.pem"));
    QFile keyFile(QStringLiteral("server.key"));
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

    addPendingConnection(serverSocket);

    connect(serverSocket, &QSslSocket::encrypted,
            this, &Receiver::ready);
    connect(serverSocket, &QSslSocket::readyRead,
            this, &Receiver::handleReadyRead);
    connect(serverSocket, &QSslSocket::disconnected,
            this, &Receiver::stopped);
    connect(this, &Receiver::receivedInfo,
            this, &Receiver::handleInfo);

    serverSocket->startServerEncryption();
}

void Receiver::ready()
{
    serverState = ServerState::ENCRYPTED;

    messenger.setDevice(serverSocket);

    RequestMessage requestID(RequestMessage::Request::DEVICE_ID);
    messenger.sendMessage(requestID);
}

void Receiver::stopped()
{
    if (serverState == ServerState::RECONNECTING)
    {
        startServer();
        return;
    }

    stopServer();
}

//-----------------------------------------------------------------------------
// Connection Methods
//-----------------------------------------------------------------------------

void Receiver::setup(RegisterDeviceList &registerDeviceList)
{
    this->registerDeviceList = &registerDeviceList;
}

void Receiver::startServer()
{
    if (serverState == ServerState::CONNECTING)
        return;

    this->listen(QHostAddress::Any, PORT);

    serverState = ServerState::CONNECTING;
}

void Receiver::stopServer()
{
    if (serverState == ServerState::DISCONNECTED)
        return;

    emit disconnected();

    if (this->isListening())
        this->close();

    if (serverState == ServerState::CONNECTING)
    {
        serverState = ServerState::DISCONNECTED;
        return;
    }

    disconnect(serverSocket, &QSslSocket::encrypted,
               this, &Receiver::ready);
    disconnect(serverSocket, &QSslSocket::readyRead,
               this, &Receiver::handleReadyRead);
    disconnect(serverSocket, &QSslSocket::disconnected,
               this, &Receiver::stopped);
    disconnect(this, &Receiver::receivedInfo,
               this, &Receiver::handleInfo);

    serverSocket->abort();

    serverState = ServerState::DISCONNECTED;
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
        default:
            break;
    }
}

void Receiver::handleInfo(std::shared_ptr<InfoMessage> info)
{
    switch (info->infoType)
    {
        case InfoMessage::InfoType::DEVICE_ID:
        {
            handleDeviceID(byteVectorToString(info->info));
            break;
        }
        default:
            break;
    }
}

void Receiver::handleDeviceID(QString deviceID)
{
    const int deviceItemsSize = registerDeviceList->items().size();

    for (int i = 1; i < deviceItemsSize; i++)
    {
        if (registerDeviceList->items().at(i).deviceID == deviceID)
        {
            AcknowledgeMessage ack(AcknowledgeMessage::Acknowledge::DEVICE_ID_OK);
            messenger.sendMessage(ack);
            emit connected();
            return;
        }
    }

    AcknowledgeMessage ack(AcknowledgeMessage::Acknowledge::ERROR);
    messenger.sendMessage(ack);

    serverState = ServerState::RECONNECTING;
}

