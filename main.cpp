#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include <QQmlContext>

#include "modules/communications/receiver.hpp"
#include "modules/communications/sender.hpp"
#include "modules/qml/register_device_list.hpp"
#include "modules/qml/register_device_model.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    qmlRegisterType<Receiver>("communications", 1, 0, "Receiver");
    qmlRegisterType<Sender>("communications", 1, 0, "Sender");
    qmlRegisterType<RegisterDeviceModel>("qml", 1, 0, "RegisterDeviceModel");
    qmlRegisterUncreatableType<RegisterDeviceList>("qml", 1, 0,
                                                   "RegisterDeviceList",
           QStringLiteral("RegisterDeviceList should not be created in QML"));

    RegisterDeviceList registerDeviceList;

    registerDeviceList.generateConf();
    registerDeviceList.readDeviceItems();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
                QStringLiteral("registerDeviceList"), &registerDeviceList);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
