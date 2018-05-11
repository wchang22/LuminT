#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include <QFile>
#include <QUuid>

#include "modules/communications/receiver.hpp"
#include "modules/communications/sender.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    qmlRegisterType<Receiver>("communications", 1, 0, "Receiver");
    qmlRegisterType<Sender>("communications", 1, 0, "Sender");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QFile configFile("lumint.conf");
    if (!configFile.exists())
    {
        configFile.open(QIODevice::WriteOnly);
        configFile.write(QUuid::createUuid()
                         .toString().toStdString().substr(1, 9).c_str(), 8);
        configFile.close();
    }

    return app.exec();
}
