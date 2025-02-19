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
#include "./Code/AppData/tcpconnection.h"
#include <QDir>
#include <QLockFile>
#ifdef WIN32
#include "./Code/AppData/installedSoftware.h"
#include <shellapi.h>
#include <windows.h>
#endif
#ifdef LINUX
#include "./Code/RDP/remotecontrol.h"
#endif

// 用于跟踪窗口是否隐藏的全局变量
bool isWindowHidden = false;

#ifdef WIN32
void requestAdminPrivileges() {
    char szPath[MAX_PATH];
    GetModuleFileNameA(NULL, szPath, MAX_PATH);
    if ((UINT_PTR)ShellExecuteA(NULL, "runas", szPath, NULL, NULL, SW_SHOW) > 32) {
        QCoreApplication::exit(0); // 提升成功后退出当前进程
    } else {
        qDebug() << "无法提升管理员权限";
    }
}
#endif

void toggleMainWindow(QQuickWindow *mainWindow) {
    if (isWindowHidden) {
        mainWindow->show();
        mainWindow->raise();
        mainWindow->requestActivate();
        isWindowHidden = false;
    } else {
        mainWindow->hide();
        isWindowHidden = true;
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 单实例检查
    QLockFile lockFile(QDir::temp().absoluteFilePath("remote_control.lock"));
    if (!lockFile.tryLock(100)) {
        qDebug() << "程序已在运行";
        return 0;
    }

#ifdef WIN32
    requestAdminPrivileges();
#endif

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/remote/Page/Main.qml"));
    Account user_account;


#ifdef LINUX

#endif

    engine.load(url);

#ifdef WIN32
    InstalledSoftware softwareManager;
    engine.rootContext()->setContextProperty("softwareManager", &softwareManager);
#endif
    tcpConnection tcpcn;
    engine.rootContext()->setContextProperty("GlobalProperties", QVariant::fromValue(GlobalProperties::getInstance()));
    engine.rootContext()->setContextProperty("account", &user_account);
    engine.rootContext()->setContextProperty("tcp", &tcpcn);

#ifdef LINUX
    RemoteControl client;
    engine.rootContext()->setContextProperty("client", &client);
#endif

    QObject *rootObject = engine.rootObjects().first();
    QQuickWindow *mainWindow = rootObject ? rootObject->findChild<QQuickWindow *>() : nullptr;
    if (!mainWindow) {
        return -1; // 处理错误：找不到窗口对象
    }

    mainWindow->setIcon(QIcon(":/images/funplayLOGO.svg"));

#ifdef Q_OS_WIN
    QSystemTrayIcon trayIcon;
    QMenu menu;
    QAction quitAction("退出", &menu);
    QAction showAction("显示", &menu);

    QObject::connect(&quitAction, &QAction::triggered, &app, &QApplication::quit);
    QObject::connect(&showAction, &QAction::triggered, [=]() {
        toggleMainWindow(mainWindow);
    });

    menu.addAction(&quitAction);
    menu.addAction(&showAction);

    trayIcon.setContextMenu(&menu);
    trayIcon.setIcon(QIcon(":/images/funplayLOGO.svg"));
    trayIcon.show();

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            toggleMainWindow(mainWindow);
        }
    });
#endif

    return app.exec();
}
