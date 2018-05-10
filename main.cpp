#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "modules/communications/receiver.hpp"
#include "modules/communications/sender.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<Receiver>("communications", 1, 0, "Receiver");
    qmlRegisterType<Sender>("communications", 1, 0, "Sender");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
