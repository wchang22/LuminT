#include "sender.hpp"
#include <QSslKey>
#include <QFile>
#include <QSslConfiguration>
#include <QDir>

const int PORT = 4002;

Sender::Sender()
{
    socketState = SenderState::DISCONNECTED;
    connect(&socket, &QSslSocket::encrypted, this, &Sender::ready);
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
    connect(&socket, &QSslSocket::disconnected, this, &Sender::stopped);
}

Sender::~Sender()
{
    socketState = SenderState::DISCONNECTED;
}

void Sender::ready()
{
    socketState = SenderState::CONNECTED;
    emit connected();
}

void Sender::stopped()
{
    socketState = SenderState::DISCONNECTED;
    emit disconnected();
}

void Sender::error(QAbstractSocket::SocketError error)
{
    if (socketState != SenderState::CONNECTING)
        return;

    if (error == QAbstractSocket::SocketError::ConnectionRefusedError)
        connectToReceiver();
}

void Sender::setup()
{
    socket.addCaCertificates("rootCA.pem");

    QList<QSslError> errorsToIgnore;
    auto serverCert = QSslCertificate::fromPath("server.pem");
    errorsToIgnore << QSslError(QSslError::HostNameMismatch, serverCert.at(0));
    socket.ignoreSslErrors(errorsToIgnore);
}

void Sender::disconnectFromReceiver()
{
    if (socketState == SenderState::DISCONNECTED)
        return;

    socketState = SenderState::DISCONNECTED;
    socket.disconnectFromHost();
}


void Sender::connectToReceiver()
{
    socket.connectToHostEncrypted("127.0.0.1", PORT);
    socketState = SenderState::CONNECTING;
}
