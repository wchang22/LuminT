#include <QFile>
#include <QNetworkInterface>

#include "sender.hpp"

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
{
    connect(&clientSocket, &QSslSocket::encrypted, this, &Sender::ready);
    connect(&clientSocket,
            qOverload<QAbstractSocket::SocketError>(&QAbstractSocket::error),
            this, &Sender::error);
    connect(&clientSocket, &QSslSocket::disconnected, this, &Sender::stopped);
}

Sender::~Sender()
{
    if (clientState != ClientState::DISCONNECTED)
        clientSocket.abort();
}

//-----------------------------------------------------------------------------
// Connection Slots
//-----------------------------------------------------------------------------

void Sender::ready()
{
    clientState = ClientState::ENCRYPTED;
    emit connected();
}

void Sender::stopped()
{
    clientState = ClientState::DISCONNECTED;
    emit disconnected();
}

void Sender::error(QAbstractSocket::SocketError error)
{
    if (clientState != ClientState::CONNECTING)
        return;

    if (error == QAbstractSocket::SocketError::ConnectionRefusedError)
        connectToReceiver();
}

//-----------------------------------------------------------------------------
// Connection Methods
//-----------------------------------------------------------------------------

void Sender::setup()
{
    clientSocket.addCaCertificates(QStringLiteral("rootCA.pem"));

    QList<QSslError> errorsToIgnore;
    auto serverCert = QSslCertificate::fromPath(QStringLiteral("server.pem"));
    errorsToIgnore << QSslError(QSslError::HostNameMismatch, serverCert.at(0));
    clientSocket.ignoreSslErrors(errorsToIgnore);
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
