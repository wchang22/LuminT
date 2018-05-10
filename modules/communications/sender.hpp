#ifndef SENDER_HPP
#define SENDER_HPP

#include <QObject>
#include <QSslSocket>

class Sender : public QObject
{
    Q_OBJECT
public:
    Sender();
    ~Sender();

    void connectToReceiver();

private slots:
    void ready();
    void sslErrors(QList<QSslError> error);
    void stateChanged(QAbstractSocket::SocketState state);
    void error(QAbstractSocket::SocketError error);

private:
    QSslSocket socket;
};

#endif // SENDER_HPP
