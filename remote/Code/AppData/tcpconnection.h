#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <string>
#include <cstring>
#include <vector>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "./devicedate.h"
#include "../LogUntils/AppLog.h"
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <czmq.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <czmq.h>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#ifdef LINUX
#include <netdb.h> // 添加 netdb.h
#include "../LogUntils/AppLog.h"
#endif
#endif

#define SERVER_PORT 5555

class tcpConnection : public QObject {
    Q_OBJECT
private:
    zsock_t * sockfd_;

public:
    tcpConnection();
    ~tcpConnection();

    Q_INVOKABLE bool connect(const QString host);

    Q_INVOKABLE bool sendPacket(const RD_Packet& packet);

    Q_INVOKABLE bool receive(RD_Packet& packet);

    void close();

    static QString TCP_IP;

    // 新增：接收远程应用列表
    Q_INVOKABLE QVariantList receiveAppList();

signals:
    void appListReceived(const QVariantList &appList);

private:
    // 新增：解析应用信息
    QVariantList parseAppList();
};

#endif // TCPCONNECTION_H
