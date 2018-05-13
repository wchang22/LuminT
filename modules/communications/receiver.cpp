#include <QSslKey>
#include <QFile>

#include "receiver.hpp"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int PORT = 4002;

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Receiver::Receiver()
    : server()
    , serverSocket(nullptr)
    , serverState(ServerState::DISCONNECTED)
{
    connect(&server, &QTcpServer::newConnection,
            this, &Receiver::connectionReceived);
    server.setMaxPendingConnections(1);
}

Receiver::~Receiver()
{
    if (server.isListening())
        server.close();

    if (!serverSocket)
    {
        serverSocket->deleteLater();
        serverSocket = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Connection Slots
//-----------------------------------------------------------------------------

void Receiver::connectionReceived()
{
    incomingConnection(server.nextPendingConnection()->socketDescriptor());
}

void Receiver::incomingConnection(qintptr serverSocketDescriptor)
{
    serverState = ServerState::ENCRYPTING;

    server.close();

    serverSocket = new QSslSocket();

    if (!serverSocket->setSocketDescriptor(serverSocketDescriptor))
    {
        delete serverSocket;
        return;
    }

    QFile certFile(QStringLiteral("server.pem"));
    QFile keyFile(QStringLiteral("server.key"));
    if (!certFile.open(QFile::ReadOnly) || !keyFile.open(QFile::ReadOnly))
    {
        delete serverSocket;
        return;
    }

    QSslCertificate cert = QSslCertificate(certFile.readAll());
    QSslKey key = QSslKey(keyFile.readAll(), QSsl::Rsa, QSsl::Pem);

    certFile.close();
    keyFile.close();

    if (key.isNull())
    {
        delete serverSocket;
        return;
    }

    serverSocket->setLocalCertificate(cert);
    serverSocket->setPrivateKey(key);

    addPendingConnection(serverSocket);

    connect(serverSocket, &QSslSocket::encrypted,
            this, &Receiver::connected);
    connect(serverSocket, &QSslSocket::readyRead,
            this, &Receiver::handleReadyRead);
    connect(serverSocket, &QSslSocket::disconnected,
            this, &Receiver::stopped);

    serverSocket->startServerEncryption();
}

void Receiver::ready()
{
    serverState = ServerState::ENCRYPTED;
    emit connected();
}

void Receiver::stopped()
{
    stopServer();
}

//-----------------------------------------------------------------------------
// Connection Methods
//-----------------------------------------------------------------------------

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

    emit disconnected();

    server.close();

    if (serverState == ServerState::CONNECTING)
    {
        serverState = ServerState::DISCONNECTED;
        return;
    }

    serverState = ServerState::DISCONNECTED;

    disconnect(serverSocket, &QSslSocket::encrypted,
               this, &Receiver::connected);
    disconnect(serverSocket, &QSslSocket::readyRead,
               this, &Receiver::handleReadyRead);
    disconnect(serverSocket, &QSslSocket::disconnected,
               this, &Receiver::stopped);

    serverSocket->abort();
}

//-----------------------------------------------------------------------------
// Read/Write
//-----------------------------------------------------------------------------

void Receiver::handleReadyRead()
{

}

