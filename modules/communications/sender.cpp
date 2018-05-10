#include "sender.hpp"
#include <QSslKey>
#include <QFile>
#include <QSslConfiguration>
#include <QDir>

const int PORT = 4002;

Sender::Sender()
{
    connect(&socket, &QSslSocket::encrypted, this, &Sender::ready);
    connect(&socket, &QSslSocket::stateChanged, this, &Sender::stateChanged);
    connect(&socket, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(sslErrors(QList<QSslError>)));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));

    connectToReceiver();
}

Sender::~Sender()
{

}

void Sender::ready()
{
    qDebug() << "Ready";
}

void Sender::sslErrors(QList<QSslError> error)
{
    for (int i = 0; i < error.size(); i++)
        qDebug() << error.at(i);
}

void Sender::error(QAbstractSocket::SocketError error)
{
    qDebug() << error;
}

void Sender::stateChanged(QAbstractSocket::SocketState state)
{

}

void Sender::connectToReceiver()
{
    socket.addCaCertificates("rootCA.pem");

    QList<QSslError> errorsToIgnore;
    auto serverCert = QSslCertificate::fromPath("server.pem");
    errorsToIgnore << QSslError(QSslError::HostNameMismatch, serverCert.at(0));
    socket.ignoreSslErrors(errorsToIgnore);

    socket.connectToHostEncrypted("127.0.0.1", PORT);
}
