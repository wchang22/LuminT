#include <QSslKey>
#include <QFile>
#include <QNetworkInterface>
#include <QStandardPaths>
#include <QDir>

#include "receiver.hpp"
#include "modules/utilities/utilities.hpp"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int PORT = 4002;
const int ENCRYPTING_TIMEOUT = 5000; // ms

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Receiver::Receiver(QObject *parent)
    : QTcpServer(parent)
    , serverSocket(nullptr)
    , serverState(ServerState::DISCONNECTED)
    , messageState(MessageState::MESSAGE)
    , messenger()
    , thisID("")
    , ipAddress("")
    , registerDeviceList(nullptr)
    , encryptingTimer(this)
    , currentFileSize(0)
    , currentPacketNumber(0)
    , currentPath(QStandardPaths::standardLocations(
                      QStandardPaths::DocumentsLocation)[0] +
                      QDir::separator())
    , currentFile()
{
    connect(this, &Receiver::receivedInfo,
            this, &Receiver::handleInfo);
    connect(this, &Receiver::receivedAcknowledge,
            this, &Receiver::handleAcknowledge);
    connect(this, &Receiver::receivedPacket,
            this, &Receiver::handlePacket);
    connect(this, &Receiver::fileCompleted,
            this, &Receiver::saveFile);

    connect(&encryptingTimer, &QTimer::timeout,
            this, &Receiver::encryptingTimeout);
    encryptingTimer.setSingleShot(true); // timer fires only once
}

Receiver::~Receiver()
{
    if (this->isListening())
        this->close();

    if (serverSocket && serverSocket->isOpen())
        serverSocket->abort();

    if (currentFile.isOpen())
        currentFile.close();
}

//-----------------------------------------------------------------------------
// Connection Slots
//-----------------------------------------------------------------------------

void Receiver::incomingConnection(qintptr serverSocketDescriptor)
{
    this->close();

    serverSocket = new QSslSocket(this);

    // Attempts to connect serverSocket to clientSocket of Sender
    if (!serverSocket->setSocketDescriptor(serverSocketDescriptor))
        return;

    connect(serverSocket, &QSslSocket::encrypted,
            this, &Receiver::socketReady);
    connect(serverSocket, &QSslSocket::readyRead,
            this, &Receiver::handleReadyRead);
    connect(serverSocket, &QSslSocket::disconnected,
            this, &Receiver::socketDisconnected);

    this->addPendingConnection(serverSocket);

    // Opens SSL certificate and key for TLS handshake
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

    // Start 5s timer for encryption
    encryptingTimer.start(ENCRYPTING_TIMEOUT);

    serverState = ServerState::ENCRYPTING;
}

void Receiver::socketReady()
{
    // Devices are now encrypted, start key verification process

    encryptingTimer.stop();

    messenger.setDevice(serverSocket);

    RequestMessage requestKey(RequestMessage::Request::DEVICE_KEY);
    messenger.sendMessage(requestKey);

    serverState = ServerState::ENCRYPTED;
}

void Receiver::socketDisconnected()
{
    if (serverState == ServerState::ENCRYPTING)
        serverState = ServerState::ERROR;

    stopServer();
}

void Receiver::encryptingTimeout()
{
    serverState = ServerState::ERROR;

    stopServer();
}

//-----------------------------------------------------------------------------
// Connection Methods
//-----------------------------------------------------------------------------

void Receiver::setup(RegisterDeviceList &registerDeviceList)
{
    ipAddress = getIPAddress();

    // If IP address is blank, internet is not connected, got into ERROR state
    if (ipAddress.length() == 0)
    {
        serverState = ServerState::ERROR;
        return;
    }

    this->thisID = ipAddress.split(".").at(3);
    this->registerDeviceList = &registerDeviceList;
}

bool Receiver::startServer()
{
    if (serverState == ServerState::CONNECTING)
        return false;

    if (!this->listen(QHostAddress(ipAddress), PORT))
        return false;

    serverState = ServerState::CONNECTING;
    messageState = MessageState::MESSAGE;
    return true;
}

void Receiver::stopServer()
{
    switch(serverState)
    {
        case ServerState::DISCONNECTED:
            return;
        case ServerState::UNRECOGNIZED:
            emit connectionUnrecognized();
            break;
        case ServerState::ERROR:
            emit connectionError();
            break;
        default:
            emit disconnected();
    }

    if (this->isListening())
        this->close();

    if (currentFile.isOpen())
        currentFile.close();

    if (encryptingTimer.isActive())
        encryptingTimer.stop();

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

QString Receiver::getIPAddress() const
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        // IP address should not be 127.0.0.1 (localhost) or end in 1,
        // which are usually gateways or IP addresses of virtual machines
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            address != QHostAddress(QHostAddress::LocalHost) &&
            address.toString().section(".", -1, -1 ) != "1")
            return address.toString();
    }

    // If there are no valid IPv4 address, return a blank string
    // Most commonly occurs when device is not connected to internet
    return QStringLiteral("");
}

QString Receiver::getThisID() const
{
    return thisID;
}

Receiver::ServerState Receiver::state() const
{
    return serverState;
}

//-----------------------------------------------------------------------------
// Read/Write
//-----------------------------------------------------------------------------

void Receiver::handleReadyRead()
{
    if (messageState == MessageState::FILE)
    {
        int64_t remainingBytes = currentFileSize -
                                  FileSize::PACKET_BYTES * currentPacketNumber;
        uint32_t expectedPacketSize = (remainingBytes >= FileSize::PACKET_BYTES) ?
                                       FileSize::PACKET_BYTES : remainingBytes;

        if (!messenger.readFile(expectedPacketSize))
            return;

        qDebug() << "2";
        emit receivedPacket(messenger.retrieveFile());

        if (remainingBytes - FileSize::PACKET_BYTES <= 0)
        {
            qDebug() << "4";
            emit fileCompleted();
            return;
        }

        qDebug() << "3";
        RequestMessage requestPacket(RequestMessage::Request::FILE_PACKET,
                                     QByteArray::number(
                                         ++currentPacketNumber));
        messenger.sendMessage(requestPacket);

        return;
    }

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
            handleDeviceKey(QString(info->info));
            break;
        }
        case InfoMessage::InfoType::FILE_INFO:
        {
            handleFileInfo(info->info);
        }
        default:
            break;
    }
}

void Receiver::handleDeviceKey(QString deviceKey)
{
    const int deviceItemsSize = registerDeviceList->items().size();

    // Check if device key is registered
    for (int i = 1; i < deviceItemsSize; i++)
    {
        if (registerDeviceList->items().at(i).deviceKey != deviceKey)
            continue;

        InfoMessage info(InfoMessage::InfoType::DEVICE_KEY,
                         registerDeviceList->getThisKey().toUtf8());
        messenger.sendMessage(info);
        return;
    }

    // If device key is not registered, send error
    AcknowledgeMessage ack(AcknowledgeMessage::Acknowledge::ERROR);
    messenger.sendMessage(ack);
    serverState = ServerState::UNRECOGNIZED;
}

void Receiver::handleFileInfo(QByteArray info)
{
    currentFileSize = Utilities::byteArrayToUint32(
                        info.left(FileSize::MAX_FILE_SIZE_REP));
    QString fileName = QString(info.mid(FileSize::MAX_FILE_SIZE_REP));

    RequestMessage requestPacket(RequestMessage::Request::FILE_PACKET,
                                 QByteArray::number(currentPacketNumber));
    messenger.sendMessage(requestPacket);

    messageState = MessageState::FILE;

    createFile(fileName);
}

void Receiver::handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack)
{
    switch (ack->ack)
    {
        case AcknowledgeMessage::Acknowledge::ERROR:
        {
            serverState = ServerState::UNRECOGNIZED;
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

void Receiver::handlePacket(std::shared_ptr<FileMessage> packet)
{
    currentFile.write(packet->fileData);
}

//-----------------------------------------------------------------------------
// File Methods
//-----------------------------------------------------------------------------

void Receiver::setFilePath(QString path)
{
    currentPath = path + QDir::separator();
}

bool Receiver::createFile(QString name)
{
    currentFile.setFileName(currentPath + name);

    if (currentFile.exists())
        return false;

    if (!currentFile.open(QIODevice::WriteOnly))
        return false;

    return true;
}

void Receiver::saveFile()
{
    if (currentFile.isOpen())
        currentFile.close();
}
