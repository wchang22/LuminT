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

Sender::Sender()
{
    connect(&clientSocket, &QSslSocket::encrypted, this, &Sender::ready);
    connect(&clientSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
    connect(&clientSocket, &QSslSocket::disconnected, this, &Sender::stopped);

    clientState = ClientState::DISCONNECTED;
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
