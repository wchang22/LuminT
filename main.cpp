#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include <QFile>
#include <QUuid>
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

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
                QStringLiteral("registerDeviceList"), &registerDeviceList);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QFile configFile(CONFIG_FILE_NAME);
    if (!configFile.exists())
    {
        configFile.open(QIODevice::WriteOnly);
        configFile.write((QUuid::createUuid()
                          .toString().toStdString()
                          .substr(1, 1+DEVICE_ID_SIZE))
                          .c_str(), DEVICE_ID_SIZE);
        configFile.close();
    }

    return app.exec();
}
