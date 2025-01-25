#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QQuickWindow>
#include <QAction>
#include <QObject>
#include "globalproperties.h"
#include "./Code/Users/account.h"
#include "./Code/DataBase/database.h"
#include "./Code/LogUntils/AppLog.h"
// 用于跟踪窗口是否隐藏的全局变量
bool isWindowHidden = false;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/remote/Page/Main.qml"));
    Account user_account;
    engine.load(url);

    // 直接传递 GlobalProperties::getInstance() 返回的指针给 setContextProperty
    engine.rootContext()->setContextProperty("GlobalProperties", QVariant::fromValue(GlobalProperties::getInstance()));
    // 注册实例到 QML
    engine.rootContext()->setContextProperty("account", &user_account);
    engine.rootContext()->setContextProperty("logger",&LoggerNamespace::globalLogger);
    // 获取QML中的窗口对象
    QObject *rootObject = engine.rootObjects().first();
    QQuickWindow *mainWindow = rootObject ? rootObject->findChild<QQuickWindow *>() : nullptr;
    if (!mainWindow) {
        // 处理错误：找不到窗口对象
        return -1;
    }

    mainWindow->setIcon(QIcon(":/images/funplayLOGO.svg"));

#ifdef Q_OS_WIN
    QSystemTrayIcon trayIcon;
    QMenu menu;
    QAction quitAction("退出", &menu);
    QAction showAction("显示", &menu);
    QObject::connect(&quitAction, &QAction::triggered, &app, &QApplication::quit);
    QObject::connect(&showAction, &QAction::triggered, [=]() {
        if (isWindowHidden) {
            mainWindow->show();
            mainWindow->raise();
            mainWindow->requestActivate();
            isWindowHidden = false;
        }
        else {
            mainWindow->hide();
            isWindowHidden = true;
        }
    });
    menu.addAction(&quitAction);
    menu.addAction(&showAction);

    trayIcon.setContextMenu(&menu);
    trayIcon.setIcon(QIcon(":/images/funplayLOGO.svg"));
    trayIcon.show();

    // 连接托盘图标的activated信号
    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            if (isWindowHidden) {
                mainWindow->show();
                mainWindow->raise();
                mainWindow->requestActivate();
                isWindowHidden = false;
            } else {
                mainWindow->hide();
                isWindowHidden = true;
                if (rootObject) {
                    QMetaObject::invokeMethod(rootObject, "minimizeToTray", Qt::QueuedConnection);
                }
            }
        }
    });
#endif

    return app.exec();
}
