#include <QQmlEngine>
#include <QQmlComponent>
#include <QDebug>

#include "register_device.hpp"

RegisterDevice::RegisterDevice()
{
    QQmlEngine engine;
    QQmlComponent component(&engine,
                            QUrl::fromLocalFile(":/RegisterPage.qml"));
    document = component.create();
    QObject* page = document->findChild<QObject*>("registerPage");
    registerButton = document->findChild<QObject*>("registerButton");
}

RegisterDevice::~RegisterDevice()
{
    delete document;
}

void RegisterDevice::addDevice()
{
    qDebug() << "hello";
}
