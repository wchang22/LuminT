#include <QSslKey>
#include <QFile>
#include <QNetworkInterface>
#include <QStandardPaths>

#include "receiver.hpp"

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Receiver::Receiver(QObject *parent)
    : QTcpServer(parent)
    , serverSocket(nullptr)
    , serverState(ServerState::DISCONNECTED)
    , messageState(MessageState::MESSAGE)
    , messenger()
    , peerKey("")
    , thisID("")
    , ipAddress("")
    , registerDeviceList(nullptr)
    , encryptingTimer(this)
    , currentFile()
    , fileTransferInfo({
        QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0] + '/', "", 0, 0, 0,
      })
{
    connect(this, &Receiver::receivedInfo, this, &Receiver::handleInfo);
    connect(this, &Receiver::receivedAcknowledge, this, &Receiver::handleAcknowledge);
    connect(this, &Receiver::receivedPacket, this, &Receiver::handlePacket);
    connect(this, &Receiver::fileCompleted, this, &Receiver::saveFile);

    connect(&encryptingTimer, &QTimer::timeout, this, &Receiver::encryptingTimeout);
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

    connect(serverSocket, &QSslSocket::encrypted, this, &Receiver::socketReady);
    connect(serverSocket, &QSslSocket::readyRead, this, &Receiver::handleReadyRead);
    connect(serverSocket, &QSslSocket::disconnected, this, &Receiver::socketDisconnected);

    this->addPendingConnection(serverSocket);

    // Opens SSL certificate and key for TLS handshake
    QFile certFile(QStringLiteral(":certificates/server.pem"));
    QFile keyFile(QStringLiteral(":certificates/server.key"));
    if (!certFile.open(QFile::ReadOnly) || !keyFile.open(QFile::ReadOnly))
        return;

    QSslCertificate cert(certFile.readAll());
    QSslKey key(keyFile.readAll(), QSsl::Rsa, QSsl::Pem);

    certFile.close();
    keyFile.close();

    if (key.isNull())
        return;

    serverSocket->setLocalCertificate(cert);
    serverSocket->setPrivateKey(key);
    serverSocket->startServerEncryption();

    // Start 5s timer for encryption
    encryptingTimer.start(LuminT::ENCRYPTING_TIMEOUT);

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

    // If IP address is blank, internet is not connected, go into ERROR state
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

    if (!this->listen(QHostAddress(ipAddress), LuminT::PORT))
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

    if (encryptingTimer.isActive())
        encryptingTimer.stop();

    if (currentFile.isOpen())
        currentFile.close();

    if (messageState == MessageState::FILE_SENDING || messageState == MessageState::FILE_PAUSED)
        saveFileTransferInfo();

    if (serverState == ServerState::CONNECTING)
    {
        serverState = ServerState::DISCONNECTED;
        return;
    }

    disconnect(serverSocket, &QSslSocket::encrypted, this, &Receiver::socketReady);
    disconnect(serverSocket, &QSslSocket::readyRead, this, &Receiver::handleReadyRead);
    disconnect(serverSocket, &QSslSocket::disconnected, this, &Receiver::socketDisconnected);

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

Receiver::MessageState Receiver::getMessageState() const
{
    return messageState;
}

QString Receiver::getCurrentPath() const
{
    return fileTransferInfo.folderPath;
}

float Receiver::getCurrentProgress() const
{
    return fileTransferInfo.progress;
}

//-----------------------------------------------------------------------------
// Handlers
//-----------------------------------------------------------------------------

void Receiver::handleReadyRead()
{
    if (messageState == MessageState::MESSAGE)
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
        return;
    }

    int64_t remainingBytes = fileTransferInfo.fileSize -
                             LuminT::PACKET_BYTES * fileTransferInfo.packetNumber;
    uint32_t expectedPacketSize = (remainingBytes >= LuminT::PACKET_BYTES) ?
                                   LuminT::PACKET_BYTES : remainingBytes;

    if (!messenger.readFile(expectedPacketSize))
        return;

    if (messageState == MessageState::FILE_ABORTING)
    {
        RequestMessage cancelFile(RequestMessage::Request::CANCEL_FILE_TRANSFER);
        messenger.sendMessage(cancelFile);

        messageState = MessageState::MESSAGE;
        return;
    }

    fileTransferInfo.progress = (double) LuminT::PACKET_BYTES * ++fileTransferInfo.packetNumber /
                                fileTransferInfo.fileSize;
    emit receivedPacket(messenger.retrieveFile());
    emit receiveProgress(fileTransferInfo.progress);

    if (remainingBytes - LuminT::PACKET_BYTES <= 0)
    {
        AcknowledgeMessage fileSuccess(AcknowledgeMessage::Acknowledge::FILE_SUCCESS);
        messenger.sendMessage(fileSuccess);

        messageState = MessageState::MESSAGE;

        clearFileTransferInfo();

        emit fileCompleted();
        return;
    }

    if (messageState == MessageState::FILE_PAUSED)
    {
        RequestMessage pauseFile(RequestMessage::Request::PAUSE_FILE_TRANSFER);
        messenger.sendMessage(pauseFile);

        currentFile.close();

        emit filePaused();
        return;
    }

    RequestMessage requestPacket(RequestMessage::Request::FILE_PACKET,
                                 QByteArray::number(fileTransferInfo.packetNumber));
    messenger.sendMessage(requestPacket);
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

void Receiver::handleDeviceKey(const QString &deviceKey)
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

        peerKey = deviceKey;

        return;
    }

    // If device key is not registered, send error
    AcknowledgeMessage ack(AcknowledgeMessage::Acknowledge::ERROR);
    messenger.sendMessage(ack);

    serverState = ServerState::UNRECOGNIZED;
}

void Receiver::handleFileInfo(QByteArray &info)
{
    fileTransferInfo.fileSize = Utilities::byteArrayToUint32(info.left(LuminT::MAX_FILE_SIZE_REP));
    fileTransferInfo.fileName = info.mid(LuminT::MAX_FILE_SIZE_REP);
    fileTransferInfo.packetNumber = 0;
    fileTransferInfo.progress = 0;

    emit receiveProgress(fileTransferInfo.progress);

    createFile(fileTransferInfo.fileName);
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
            FileTransferInfo retrievedFileTransferInfo = retrieveFileTransferInfo();

            if (retrievedFileTransferInfo.folderPath.length() == 0)
            {
                emit connected();
                return;
            }

            fileTransferInfo = retrievedFileTransferInfo;
            messageState = MessageState::FILE_PAUSED;

            currentFile.setFileName(fileTransferInfo.folderPath +
                                    fileTransferInfo.fileName);

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

void Receiver::setFilePath(const QString &path)
{
    fileTransferInfo.folderPath = path + '/';
}

void Receiver::createFile(const QString &name)
{
    if (name.length() == 0)
    {
        emit fileStatus(FileState::ERROR, name);
        return;
    }

    currentFile.setFileName(fileTransferInfo.folderPath + name);

    if (currentFile.exists())
    {
        emit fileStatus(FileState::EXISTS, name);
        return;
    }

    if (!currentFile.open(QIODevice::WriteOnly))
    {
        emit fileStatus(FileState::ERROR, name);
        return;
    }

    emit fileStatus(FileState::OK, name);
}

void Receiver::saveFile()
{
    if (currentFile.isOpen())
        currentFile.close();
}

void Receiver::sendFileError()
{
    AcknowledgeMessage fileError(AcknowledgeMessage::Acknowledge::FILE_ERROR);
    messenger.sendMessage(fileError);

    messageState = MessageState::MESSAGE;
}

void Receiver::requestFirstPacket()
{
    RequestMessage requestPacket(RequestMessage::Request::FILE_PACKET, QByteArray::number(0));
    messenger.sendMessage(requestPacket);

    messageState = MessageState::FILE_SENDING;
}

void Receiver::pauseFileTransfer()
{
    if (messageState != MessageState::FILE_SENDING)
        return;

    messageState = MessageState::FILE_PAUSED;
}

void Receiver::resumeFileTransfer()
{
    if (messageState != MessageState::FILE_PAUSED)
        return;

    currentFile.open(QIODevice::WriteOnly | QIODevice::Append);
    currentFile.seek(fileTransferInfo.packetNumber * LuminT::PACKET_BYTES);

    RequestMessage requestPacket(RequestMessage::Request::FILE_PACKET,
                                 QByteArray::number(fileTransferInfo.packetNumber));
    messenger.sendMessage(requestPacket);

    messageState = MessageState::FILE_SENDING;

    emit fileResumed();
}

void Receiver::cancelFileTransfer()
{
    if (messageState == MessageState::MESSAGE)
        return;

    saveFile();
    currentFile.remove();

    fileTransferInfo.progress = 0;
    emit receiveProgress(fileTransferInfo.progress);

    if (messageState != MessageState::FILE_PAUSED)
    {
        messageState = MessageState::FILE_ABORTING;
        return;
    }

    RequestMessage cancelFile(RequestMessage::Request::CANCEL_FILE_TRANSFER);
    messenger.sendMessage(cancelFile);

    messageState = MessageState::MESSAGE;
}

void Receiver::saveFileTransferInfo()
{
    QFile fileTransferInfoFile(LuminT::RECEIVER_FILE_TRANSFER_INFO_NAME);
    fileTransferInfoFile.open(QIODevice::ReadOnly);

    QDataStream file(&fileTransferInfoFile);
    QMap<QString, FileTransferInfo> fileTransferInfoMap;
    file >> fileTransferInfoMap;

    fileTransferInfoMap.insert(peerKey, fileTransferInfo);

    fileTransferInfoFile.close();
    fileTransferInfoFile.open(QIODevice::WriteOnly);
    file << fileTransferInfoMap;
    fileTransferInfoFile.close();
}

Receiver::FileTransferInfo Receiver::retrieveFileTransferInfo()
{
    QFile fileTransferInfoFile(LuminT::RECEIVER_FILE_TRANSFER_INFO_NAME);

    if (!fileTransferInfoFile.exists() || fileTransferInfoFile.size() == 0)
        return { "", "", 0, 0, 0 };

    fileTransferInfoFile.open(QIODevice::ReadOnly);
    QDataStream file(&fileTransferInfoFile);

    QMap<QString, FileTransferInfo> fileTransferInfoMap;

    file >> fileTransferInfoMap;
    fileTransferInfoFile.close();

    if (fileTransferInfoMap.find(peerKey) == fileTransferInfoMap.end())
        return { "", "", 0, 0, 0 };

    return fileTransferInfoMap[peerKey];
}

void Receiver::clearFileTransferInfo()
{
    QFile fileTransferInfoFile(LuminT::RECEIVER_FILE_TRANSFER_INFO_NAME);
    fileTransferInfoFile.open(QIODevice::ReadOnly);

    QDataStream file(&fileTransferInfoFile);
    QMap<QString, FileTransferInfo> fileTransferInfoMap;
    file >> fileTransferInfoMap;

    fileTransferInfoMap.remove(peerKey);

    fileTransferInfoFile.close();
    fileTransferInfoFile.open(QIODevice::WriteOnly);
    file << fileTransferInfoMap;
    fileTransferInfoFile.close();
}

//-----------------------------------------------------------------------------
// Overloaded Operators
//-----------------------------------------------------------------------------

QDataStream &operator<<(QDataStream &out, const QMap<QString, Receiver::FileTransferInfo> &map)
{
    QMap<QString, Receiver::FileTransferInfo>::const_iterator i = map.constBegin();

    while (i != map.constEnd())
    {
        out << i.key() << i.value().folderPath << i.value().fileName << i.value().fileSize
            << i.value().packetNumber << i.value().progress;
        i++;
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, QMap<QString, Receiver::FileTransferInfo> &map)
{
    QString key;
    QString folderPath;
    QString fileName;
    uint32_t fileSize;
    uint32_t packetNumber;
    float progress;

    while (!in.atEnd())
    {
        in >> key >> folderPath >> fileName >> fileSize >> packetNumber >> progress;
        map.insert(key, { folderPath, fileName, fileSize, packetNumber, progress });
    }

    return in;
}

#include "moc_receiver.cpp"
