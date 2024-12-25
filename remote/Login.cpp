#include <QApplication> // 替换 QGuiApplication
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "globalproperties.h"
#include "account.h"
#include "database.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv); // 使用 QApplication

    QQmlApplicationEngine engine;
    Account user_account;

    const QUrl url(QStringLiteral("qrc:/remote/Page/Main.qml"));

    QCoreApplication::setQuitLockEnabled(true);

    // 直接传递 GlobalProperties::getInstance() 返回的指针给 setContextProperty
    engine.rootContext()->setContextProperty("GlobalProperties", QVariant::fromValue(GlobalProperties::getInstance()));
    // 注册实例到 QML
    engine.rootContext()->setContextProperty("account", &user_account);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
