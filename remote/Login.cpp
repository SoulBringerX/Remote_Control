#include <QApplication> // 替换 QGuiApplication
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QObject>
#include "globalproperties.h"
#include "./Code/Users/account.h"
#include "./Code/DataBase/database.h"

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

    #ifdef Q_OS_WIN
        QSystemTrayIcon trayIcon;
        QMenu menu;
        QAction quitAction("退出", &menu);
        QObject::connect(&quitAction, &QAction::triggered, &app, &QApplication::quit);
        menu.addAction(&quitAction);

        trayIcon.setContextMenu(&menu);
        trayIcon.setIcon(QIcon(":/images/tray_icon.png"));
        trayIcon.show();
    #endif
    
    return app.exec();
}
