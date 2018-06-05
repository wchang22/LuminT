#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include <QQmlContext>

#include "modules/communications/receiver.hpp"
#include "modules/communications/sender.hpp"
#include "modules/qml/register_device_list.hpp"
#include "modules/qml/register_device_model.hpp"
#ifdef Q_OS_ANDROID
    #include "modules/android/android_permissions.hpp"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // Disable Ssl warnings
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    // Create qml objects of main classes
    qmlRegisterUncreatableType<Receiver>("communications", 1, 0, "Receiver",
        QStringLiteral("Receiver should not be created in QML"));

    qmlRegisterUncreatableType<Sender>("communications", 1, 0, "Sender",
        QStringLiteral("Sender should not be created in QML"));

    qmlRegisterUncreatableType<RegisterDeviceList>("qml", 1, 0, "RegisterDeviceList",
        QStringLiteral("RegisterDeviceList should not be created in QML"));

    qmlRegisterType<RegisterDeviceModel>("qml", 1, 0, "RegisterDeviceModel");

    // Setup objects
    RegisterDeviceList registerDeviceList;
    Sender sender;
    Receiver receiver;

    registerDeviceList.generateConf();
    registerDeviceList.readDeviceItems();
    sender.setup(registerDeviceList.getThisKey(), registerDeviceList);
    receiver.setup(registerDeviceList.getThisKey(), registerDeviceList);

    // Expose objects to qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
                QStringLiteral("registerDeviceList"), &registerDeviceList);
    engine.rootContext()->setContextProperty(QStringLiteral("sender"), &sender);
    engine.rootContext()->setContextProperty(QStringLiteral("receiver"), &receiver);

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

#ifdef Q_OS_ANDROID
    AndroidPermissions permissions;
    permissions.requestPermissions();
#endif

    return app.exec();
}
