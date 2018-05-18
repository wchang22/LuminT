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
    , thisID("")
{
    connect(&clientSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
    connect(&clientSocket, &QSslSocket::encrypted, this, &Sender::ready);
    connect(&clientSocket, &QSslSocket::disconnected, this, &Sender::stopped);
    connect(&clientSocket, &QSslSocket::readyRead, this, &Sender::handleReadyRead);
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

void Sender::ready()
{
    clientState = ClientState::ENCRYPTED;
    messenger.setDevice(&clientSocket);
}

void Sender::stopped()
{
    if (clientState == ClientState::RECONNECTING)
        return;

    clientState = ClientState::DISCONNECTED;
    emit disconnected();
}

void Sender::error(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::SocketError::ConnectionRefusedError)
        connectToReceiver();
}

//-----------------------------------------------------------------------------
// Connection Methods
//-----------------------------------------------------------------------------

void Sender::setup(QString thisID)
{
    clientSocket.addCaCertificates(QStringLiteral("rootCA.pem"));

    QList<QSslError> errorsToIgnore;
    auto serverCert = QSslCertificate::fromPath(QStringLiteral("server.pem"));
    errorsToIgnore << QSslError(QSslError::HostNameMismatch, serverCert.at(0));
    clientSocket.ignoreSslErrors(errorsToIgnore);

    this->thisID = thisID;
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

void Sender::disconnectFromReceiver()
{
    if (clientState == ClientState::DISCONNECTED)
        return;

    clientSocket.abort();
    stopped();
}

void Sender::connectToReceiver()
{
    clientSocket.connectToHostEncrypted(getIPAddress(), PORT);
    clientState = ClientState::CONNECTING;
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

void Sender::handleRequest(std::shared_ptr<RequestMessage> request)
{
    switch (request->request)
    {
        case RequestMessage::Request::DEVICE_ID:
        {
            InfoMessage info(InfoMessage::InfoType::DEVICE_ID, stringToByteVector(thisID));
            messenger.sendMessage(info);
            break;
        }
        default:
            break;
    }
}

void Sender::handleAcknowledge(std::shared_ptr<AcknowledgeMessage> ack)
{
    if (ack->ack == AcknowledgeMessage::Acknowledge::DEVICE_ID_OK)
    {
        emit connected();
        return;
    }

    clientState = ClientState::RECONNECTING;
    clientSocket.abort();
    connectToReceiver();
}
