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

    enum class SenderState : int
    {
        DISCONNECTED        = 0,
        CONNECTING          = 1,
        CONNECTED           = 2,
    };

signals:
    void connected();
    void disconnected();

public slots:
    void connectToReceiver();
    void disconnectFromReceiver();
    void setup();

private slots:
    void ready();
    void stopped();
    void error(QAbstractSocket::SocketError error);

private:
    QSslSocket socket;
    SenderState socketState;
};

#endif // SENDER_HPP
