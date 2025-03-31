#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QThread>
#include <QVariantList>
#include <czmq.h>
#include <iostream>
#include "../AppData/devicedate.h"
#include "../AppData/deviceinfomanager.h"
#include "../LogUntils/AppLog.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <QVariantMap>
#endif

#define SERVER_PORT 5555

class tcpConnection : public QObject {
    Q_OBJECT
public:
    explicit tcpConnection(QObject *parent = nullptr);
    ~tcpConnection();

    Q_INVOKABLE bool connectToServer(const QString &host);
    Q_INVOKABLE bool sendPacket(const RD_Packet& packet);
    Q_INVOKABLE bool receive(RD_Packet &packet);
    void close();

    Q_INVOKABLE QVariantList receiveAppList();
    Q_INVOKABLE QVariantMap receiveDeviceInfo();
    Q_INVOKABLE QString receiveAppPath(const QString& AppName);
    Q_INVOKABLE QString receiveUninstallAppPath(const QString& AppName);
    Q_INVOKABLE bool sendInstallPackage(const QString& filePath);
    QVariantList parseAppList();

signals:
    void appListReceived(const QVariantList &appList);
    void deviceInfoReceived(QVariantMap deviceInfo);
    void connectionError(QString error); // 连接错误信号

private:
    zsock_t *sockfd_ = nullptr;
    static QString TCP_IP;
};

class TcpThread : public QThread {
    Q_OBJECT
public:
    explicit TcpThread(QObject *parent = nullptr);
    ~TcpThread();

    tcpConnection *getTcpConnection();

protected:
    void run() override; // 线程入口
signals:
    void tcpReady(tcpConnection* tcpConn);
private:
    tcpConnection *tcpConn;
};

#endif // TCPCONNECTION_H
