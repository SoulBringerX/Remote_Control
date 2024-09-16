#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "globalproperties.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/remote/Main.qml"));

    QCoreApplication::setQuitLockEnabled(true);

    // 直接传递 GlobalProperties::getInstance() 返回的指针给 setContextProperty
    engine.rootContext()->setContextProperty("GlobalProperties", QVariant::fromValue(GlobalProperties::getInstance()));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
