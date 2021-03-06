#include <QFile>
#include <QNetworkInterface>
#include <QDir>
#include <QDataStream>

#include "sender.hpp"
#include "modules/qml/register_device_list.hpp"
#include "modules/message/text_message.hpp"
#include "modules/message/file_message.hpp"

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Sender::Sender(QObject *parent)
    : QObject(parent)
    , clientSocket()
    , clientState(ClientState::DISCONNECTED)
    , messageState(MessageState::MESSAGE)
    , messenger()
    , peerKey("")
    , peerIPAddress("")
    , registerDeviceList(nullptr)
    , encryptingTimer(this)
    , fileTransferInfo({ "", 0, 0 })
{
    connect(&clientSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&clientSocket, &QSslSocket::connected, this, &Sender::socketConnected);
    connect(&clientSocket, &QSslSocket::encrypted, this, &Sender::socketReady);
    connect(&clientSocket, &QSslSocket::disconnected, this, &Sender::socketDisconnected);
    connect(&clientSocket, &QSslSocket::readyRead, this, &Sender::handleReadyRead);
    connect(this, &Sender::receivedInfo, this, &Sender::handleInfo);
    connect(this, &Sender::receivedRequest, this, &Sender::handleRequest);
    connect(this, &Sender::receivedAcknowledge, this, &Sender::handleAcknowledge);

    connect(&encryptingTimer, &QTimer::timeout, this, &Sender::encryptingTimeout);
    encryptingTimer.setSingleShot(true); // timer fires only once
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
    encryptingTimer.start(LuminT::ENCRYPTING_TIMEOUT);

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

    if (messageState == MessageState::FILE_SENDING || messageState == MessageState::FILE_PAUSED)
        saveFileTransferInfo();
}

void Sender::socketError(QAbstractSocket::SocketError error)
{
    // Receiver has not started listening yet, continue attempting to connect
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

void Sender::setup(RegisterDeviceList &registerDeviceList)
{
    // Sets up CA certificate
    clientSocket.addCaCertificates(QStringLiteral(":certificates/rootCA.pem"));

    // Does not matter if Receiver's hostname does not match the one listed in
    // the certificate
    QList<QSslError> errorsToIgnore;
    QList<QSslCertificate> serverCert =
        QSslCertificate::fromPath(QStringLiteral(":certificates/server.pem"));
    errorsToIgnore << QSslError(QSslError::HostNameMismatch, serverCert.at(0));
    clientSocket.ignoreSslErrors(errorsToIgnore);

    this->registerDeviceList = &registerDeviceList;
}

void Sender::connectToReceiver()
{
    clientSocket.connectToHostEncrypted(peerIPAddress, LuminT::PORT);

    clientState = ClientState::CONNECTING;

    messageState = MessageState::MESSAGE;
}

void Sender::disconnectFromReceiver()
{
    if (clientState == ClientState::DISCONNECTED)
        return;

    clientSocket.abort();
    clientState = ClientState::DISCONNECTED;
}

void Sender::setPeerIPAddress(const QString &peerID)
{
    // Replace the last part of Sender's IP address with Receiver's ID
    // to obtain Receiver's IP address
    QStringList ipStrList = getIPAddress().split(".");
    ipStrList.replace(3, peerID);
    peerIPAddress = ipStrList.join(".");
}

QString Sender::getIPAddress() const
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

Sender::MessageState Sender::getMessageState() const
{
    return messageState;
}

QString Sender::getCurrentPath() const
{
    return fileTransferInfo.filePath;
}

float Sender::getCurrentProgress() const
{
    return fileTransferInfo.progress;
}

//-----------------------------------------------------------------------------
// Handlers
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

void Sender::handleDeviceKey(const QString &deviceKey)
{
    const int deviceItemsSize = registerDeviceList->items().size();

    // Check if device key is registered
    for (int i = 1; i < deviceItemsSize; i++)
    {
        if (registerDeviceList->items().at(i).deviceKey != deviceKey)
            continue;

        AcknowledgeMessage ack(AcknowledgeMessage::Acknowledge::DEVICE_KEY_OK);
        messenger.sendMessage(ack);

        peerKey = deviceKey;

        FileTransferInfo retrievedFileTransferInfo = retrieveFileTransferInfo();

        if (retrievedFileTransferInfo.filePath.length() != 0)
        {
            fileTransferInfo = retrievedFileTransferInfo;
            messageState = MessageState::FILE_PAUSED;
        }

        emit connected();
        return;
    }

    // If device key is not registered, send error
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
                             registerDeviceList->getThisKey().toUtf8());
            messenger.sendMessage(info);
            break;
        }
        case RequestMessage::Request::FILE_PACKET:
        {
            if (messageState == MessageState::MESSAGE)
                return;

            uint32_t packetNumber = static_cast<uint32_t>(request->requestInfo.toULong());
            FileMessage filePacket(fileTransferInfo.filePath, packetNumber);
            messenger.sendMessage(filePacket);

            fileTransferInfo.progress = (float) ++packetNumber *
                                        LuminT::PACKET_BYTES / fileTransferInfo.fileSize;
            emit sendProgress(fileTransferInfo.progress);
            break;
        }
        case RequestMessage::Request::CANCEL_FILE_TRANSFER:
        {
            messageState = MessageState::MESSAGE;

            fileTransferInfo.progress = 0;

            clearFileTransferInfo();

            emit fileCompleted();
            emit sendProgress(fileTransferInfo.progress);
            break;
        }
        case RequestMessage::Request::PAUSE_FILE_TRANSFER:
        {
            messageState = MessageState::FILE_PAUSED;
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
        case AcknowledgeMessage::Acknowledge::FILE_ERROR:
        {
            messageState = MessageState::MESSAGE;
            emit fileError();
            break;
        }
        case AcknowledgeMessage::Acknowledge::FILE_SUCCESS:
        {
            messageState = MessageState::MESSAGE;

            clearFileTransferInfo();

            emit fileCompleted();
            break;
        }
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
// Read/Write
//-----------------------------------------------------------------------------

bool Sender::sendTextMessage(const QString &text)
{
    if (messageState == MessageState::FILE_SENDING)
        return false;

    TextMessage textMessage(text);
    if (!messenger.sendMessage(textMessage))
        return false;

    return true;
}

bool Sender::sendFile(const QString &filePath)
{
    if (messageState == MessageState::FILE_SENDING)
        return false;

    fileTransferInfo.filePath = filePath;

    QFile file(fileTransferInfo.filePath);
    qint64 fileSize = file.size();

    // Todo: Error if max file size exceeded
    if (fileSize > LuminT::MAX_FILE_SIZE || fileSize <= 0)
        return false;

    fileTransferInfo.fileSize = fileSize;

    QString fileName(fileTransferInfo.filePath.split("/").last());

    QByteArray info;
    info.append(Utilities::uint32ToByteArray(fileTransferInfo.fileSize));
    info.append(fileName);

    InfoMessage fileInfo(InfoMessage::InfoType::FILE_INFO, info);

    if (!messenger.sendMessage(fileInfo))
        return false;

    fileTransferInfo.progress = 0;
    emit sendProgress(fileTransferInfo.progress);

    messageState = MessageState::FILE_SENDING;

    return true;
}

void Sender::saveFileTransferInfo()
{
    QFile fileTransferInfoFile(LuminT::SENDER_FILE_TRANSFER_INFO_NAME);
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

Sender::FileTransferInfo Sender::retrieveFileTransferInfo()
{
    QFile fileTransferInfoFile(LuminT::SENDER_FILE_TRANSFER_INFO_NAME);

    if (!fileTransferInfoFile.exists() || fileTransferInfoFile.size() == 0)
        return { "", 0, 0 };

    fileTransferInfoFile.open(QIODevice::ReadOnly);
    QDataStream file(&fileTransferInfoFile);

    QMap<QString, FileTransferInfo> fileTransferInfoMap;

    file >> fileTransferInfoMap;
    fileTransferInfoFile.close();

    if (fileTransferInfoMap.find(peerKey) == fileTransferInfoMap.end())
        return { "", 0, 0 };

    return fileTransferInfoMap[peerKey];
}

void Sender::clearFileTransferInfo()
{
    QFile fileTransferInfoFile(LuminT::SENDER_FILE_TRANSFER_INFO_NAME);
    fileTransferInfoFile.open(QIODevice::ReadOnly);

    QDataStream file(&fileTransferInfoFile);
    QMap<QString, FileTransferInfo> fileTransferInfoMap;
    file >> fileTransferInfoMap;

    fileTransferInfoMap.remove(peerKey);

    fileTransferInfoFile.close();
    fileTransferInfoFile.open(QIODevice::WriteOnly);
    file << fileTransferInfoMap;
    fileTransferInfoFile.close();

    fileTransferInfo = { "", 0, 0 };
}

//-----------------------------------------------------------------------------
// Overloaded Operators
//-----------------------------------------------------------------------------

QDataStream &operator<<(QDataStream &out, const QMap<QString, Sender::FileTransferInfo> &map)
{
    QMap<QString, Sender::FileTransferInfo>::const_iterator i = map.constBegin();

    while (i != map.constEnd())
    {
        out << i.key() << i.value().filePath << i.value().fileSize << i.value().progress;
        i++;
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, QMap<QString, Sender::FileTransferInfo> &map)
{
    QString key;
    QString filePath;
    uint32_t fileSize;
    float progress;

    while (!in.atEnd())
    {
        in >> key >> filePath >> fileSize >> progress;
        map.insert(key, { filePath, fileSize, progress });
    }

    return in;
}

#include "moc_sender.cpp"
