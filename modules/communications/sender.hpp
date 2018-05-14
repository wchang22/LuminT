#ifndef SENDER_HPP
#define SENDER_HPP

#include <QObject>
#include <QSslSocket>

class Sender : public QObject
{
    Q_OBJECT
public:
    explicit Sender(QObject *parent = nullptr);
    ~Sender();

    enum class ClientState : uint8_t
    {
        DISCONNECTED        = 0,
        CONNECTING          = 1,
        ENCRYPTED           = 2,
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
    QString getIPAddress() const;

    QSslSocket clientSocket;
    ClientState clientState;
};

#endif // SENDER_HPP
