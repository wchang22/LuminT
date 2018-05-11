#include "receiver.hpp"
#include <QSslKey>
#include <QFile>

const int PORT = 4002;

Receiver::Receiver()
{
    socket = nullptr;
    serverState = ServerState::DISCONNECTED;
    server.setMaxPendingConnections(1);
    connect(&server, &QTcpServer::newConnection,
            this, &Receiver::connectionReceived);
}

Receiver::~Receiver()
{
    serverState = ServerState::DISCONNECTED;
    if (socket != nullptr)
        socket->deleteLater();
}

void Receiver::startServer()
{
    if (serverState == ServerState::CONNECTING)
        return;

    server.listen(QHostAddress::Any, PORT);

    serverState = ServerState::CONNECTING;
}

void Receiver::stopServer()
{
    if (serverState == ServerState::DISCONNECTED)
        return;

    server.close();
    serverState = ServerState::DISCONNECTED;

    if (serverState == ServerState::CONNECTING)
        return;

    disconnect(socket, &QSslSocket::encrypted, this, &Receiver::connected);
    disconnect(socket, &QSslSocket::readyRead, this, &Receiver::handleReadyRead);
    disconnect(socket, &QSslSocket::disconnected, this, &Receiver::stopped);

    socket->deleteLater();
    socket = nullptr;
}

void Receiver::connectionReceived()
{
    incomingConnection(server.nextPendingConnection()->socketDescriptor());
}

void Receiver::incomingConnection(qintptr socketDescriptor)
{
    server.pauseAccepting();

    serverState = ServerState::ENCRYPTING;

    socket = new QSslSocket();

    if (!socket->setSocketDescriptor(socketDescriptor))
        return;

    QFile certFile("server.pem");
    QFile keyFile("server.key");
    if (!certFile.open(QFile::ReadOnly) || !keyFile.open(QFile::ReadOnly))
        return;

    QSslCertificate cert = QSslCertificate(certFile.readAll());
    QSslKey key = QSslKey(keyFile.readAll(), QSsl::Rsa, QSsl::Pem);

    certFile.close();
    keyFile.close();

    if (key.isNull())
        return;

    socket->setLocalCertificate(cert);
    socket->setPrivateKey(key);
    server.close();

    addPendingConnection(socket);

    connect(socket, &QSslSocket::encrypted, this, &Receiver::connected);
    connect(socket, &QSslSocket::readyRead, this, &Receiver::handleReadyRead);
    connect(socket, &QSslSocket::disconnected, this, &Receiver::stopped);

    socket->startServerEncryption();
}

void Receiver::ready()
{
    serverState = ServerState::ENCRYPTED;
    emit connected();
}

void Receiver::stopped()
{
    stopServer();
    emit disconnected();
}

void Receiver::handleReadyRead()
{

}

