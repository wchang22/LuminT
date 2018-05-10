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

private slots:
     void incomingConnection();
     void connected();
     void handleReadyRead();

private:
    QTcpServer server;
    QSslSocket *socket;
};

#endif // RECEIVER_HPP
