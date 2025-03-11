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
#include "./Code/TCP/tcpconnection.h"
#include "./Code/TCP/tcpserverthread.h"
#include <QDir>
#include <QLockFile>
#ifdef WIN32
#include "./Code/AppData/installedSoftware.h"
#include <shellapi.h>
#include <windows.h>
#endif
#ifdef LINUX
#include "./Code/RDP/remotecontrol.h"
#include "./Code/RDP/remotecontrolthread.h"
#include "./Code/RDP/remoteimageprovider.h"
#include "./Code/Users/userdevice.h"
#endif

bool isWindowHidden = false;

#ifdef WIN32
void requestAdminPrivileges() {
    char szPath[MAX_PATH];
    GetModuleFileNameA(NULL, szPath, MAX_PATH);
    if ((UINT_PTR)ShellExecuteA(NULL, "runas", szPath, NULL, NULL, SW_SHOW) > 32) {
        QCoreApplication::exit(0);
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

    // 防止程序多次运行
    QLockFile lockFile(QDir::temp().absoluteFilePath("remote_control.lock"));
    if (!lockFile.tryLock(100)) {
        qDebug() << "程序已在运行";
        return 0;
    }

#ifdef WIN32
    // 请求管理员权限
    requestAdminPrivileges();
#endif

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/remote/Page/Main.qml"));
    Account user_account;

    engine.load(url);

#ifdef WIN32
    // Windows平台的软件管理器
    InstalledSoftware softwareManager;
    softwareManager.refreshSoftwareList();
    engine.rootContext()->setContextProperty("softwareManager", &softwareManager);
#endif

    // 创建 TCP 线程
    TcpThread *tcpThread = new TcpThread();
    QObject::connect(tcpThread, &TcpThread::tcpReady, tcpThread, [&engine](tcpConnection* tcp) {
        engine.rootContext()->setContextProperty("tcp", tcp);  // 绑定 tcp 到 QML
    });
    tcpThread->start();  // 启动 TCP 线程

    // 绑定其他必要的上下文属性
    engine.rootContext()->setContextProperty("GlobalProperties", QVariant::fromValue(GlobalProperties::getInstance()));
    engine.rootContext()->setContextProperty("account", &user_account);

#ifdef LINUX
    // Linux 平台的远程控制功能
    RemoteControl client;
    RemoteImageProvider* imageProvider = new RemoteImageProvider(&client);
    UserDevice userdevice;
    RemoteControlThread *thread = new RemoteControlThread(nullptr, &client);
    engine.rootContext()->setContextProperty("client", &client);
    engine.rootContext()->setContextProperty("user_device", &userdevice);
    engine.addImageProvider("remote", imageProvider);
    engine.rootContext()->setContextProperty("remoteControlThread", thread);
#endif

    // 获取 QML 中的主窗口
    QObject *rootObject = engine.rootObjects().first();
    QQuickWindow *mainWindow = rootObject ? rootObject->findChild<QQuickWindow *>() : nullptr;
    if (!mainWindow) {
        return -1;
    }

    mainWindow->setIcon(QIcon(":/images/funplayLOGO.svg"));

#ifdef Q_OS_WIN
    // Windows 系统托盘图标
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

#ifdef Q_OS_WIN
    // TCP 服务线程
    TcpServerThread *tcpServerThread = new TcpServerThread();
    tcpServerThread->start();

    QObject::connect(&app, &QApplication::aboutToQuit, [=]() {
        tcpServerThread->stop();
        tcpServerThread->quit();
        tcpServerThread->wait(2000);
        delete tcpServerThread;

        // 关闭 TCP 线程
        tcpThread->quit();
        tcpThread->wait(2000);
        delete tcpThread;
    });
#endif

    return app.exec();
}
