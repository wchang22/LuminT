#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <QTcpServer>
#include <QSslSocket>

class Receiver : public QTcpServer
{
    Q_OBJECT
public:
    Receiver();
    ~Receiver();

    enum class ServerState : int
    {
        DISCONNECTED        = 0,
        CONNECTING          = 1,
        ENCRYPTING          = 2,
        ENCRYPTED           = 3,
    };

signals:
    void connected();
    void disconnected();

public slots:
     void startServer();
     void stopServer();

private slots:
     void incomingConnection(qintptr socketDescriptor);
     void connectionReceived();
     void ready();
     void stopped();
     void handleReadyRead();

private:
    QTcpServer server;
    QSslSocket *serverSocket;
    ServerState serverState;
};

#endif // RECEIVER_HPP
