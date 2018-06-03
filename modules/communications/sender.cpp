#include <QFile>
#include <QNetworkInterface>

#include "sender.hpp"
#include "modules/qml/register_device_list.hpp"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int PORT = 4002;

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Sender::Sender(QObject *parent)
    : QObject(parent)
    , clientSocket()
    , clientState(ClientState::DISCONNECTED)
    , messenger()
    , thisKey("")
    , registerDeviceList(nullptr)
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
    clientState = ClientState::CONNECTED;
}

void Sender::socketReady()
{
    messenger.setDevice(&clientSocket);
    clientState = ClientState::ENCRYPTED;
}

void Sender::socketDisconnected()
{
    if (clientState == ClientState::RECONNECTING)
    {
        connectToReceiver();
        return;
    }

    clientState = ClientState::DISCONNECTED;
    emit disconnected();
}

void Sender::socketError(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::SocketError::ConnectionRefusedError)
        connectToReceiver();
}

//-----------------------------------------------------------------------------
// Connection Methods
//-----------------------------------------------------------------------------

void Sender::setup(QString thisKey, RegisterDeviceList &registerDeviceList)
{
    clientSocket.addCaCertificates(QStringLiteral("rootCA.pem"));

    QList<QSslError> errorsToIgnore;
    auto serverCert = QSslCertificate::fromPath(QStringLiteral("server.pem"));
    errorsToIgnore << QSslError(QSslError::HostNameMismatch, serverCert.at(0));
    clientSocket.ignoreSslErrors(errorsToIgnore);

    this->thisKey = thisKey;
    this->thisID = getIPAddress().split(".").at(3);
    this->registerDeviceList = &registerDeviceList;
}

void Sender::connectToReceiver()
{
    clientSocket.connectToHostEncrypted(getIPAddress(), PORT);

    clientState = ClientState::CONNECTING;
}

void Sender::disconnectFromReceiver()
{
    if (clientState == ClientState::DISCONNECTED)
        return;

    clientSocket.abort();
    socketDisconnected();
}

QString Sender::getIPAddress() const
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            address != QHostAddress(QHostAddress::LocalHost))
            return address.toString();
    }

    return "";
}

QString Sender::getThisID() const
{
    return thisID;
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
            handleDeviceKey(byteVectorToString(info->info));
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

    clientState = ClientState::RECONNECTING;
}

void Sender::handleRequest(std::shared_ptr<RequestMessage> request)
{
    switch (request->request)
    {
        case RequestMessage::Request::DEVICE_KEY:
        {
            InfoMessage info(InfoMessage::InfoType::DEVICE_KEY,
                             stringToByteVector(thisKey));
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
            clientState = ClientState::RECONNECTING;
            clientSocket.disconnectFromHost();
            break;
        }
        default:
            break;
    }
}
