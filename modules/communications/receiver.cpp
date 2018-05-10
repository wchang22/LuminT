#include "receiver.hpp"
#include <QSslKey>
#include <QFile>

const int PORT = 4002;

Receiver::Receiver()
{
    server.setMaxPendingConnections(1);
    server.listen(QHostAddress::Any, PORT);

    socket = nullptr;

    connect(&server, &QTcpServer::newConnection,
            this, &Receiver::incomingConnection);
}

Receiver::~Receiver()
{
    if (socket != nullptr)
        socket->deleteLater();
}

void Receiver::incomingConnection()
{
    qintptr socketDescriptor = server.nextPendingConnection()->socketDescriptor();

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

    addPendingConnection(socket);
    server.pauseAccepting();

    connect(socket, &QSslSocket::encrypted, this, &Receiver::connected);
    connect(socket, &QSslSocket::readyRead, this, &Receiver::handleReadyRead);

    socket->startServerEncryption();
}

void Receiver::connected()
{
    qDebug() << "Connected";
}

void Receiver::handleReadyRead()
{

}

