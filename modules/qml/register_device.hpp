#ifndef REGISTER_DEVICE_HPP
#define REGISTER_DEVICE_HPP

#include <QObject>

class RegisterDevice : public QObject
{
    Q_OBJECT
public:
    RegisterDevice();
    ~RegisterDevice();

signals:

public slots:
    void addDevice();

private:
    QObject *document;
    QObject *registerButton;
};

#endif // REGISTER_DEVICE_HPP
