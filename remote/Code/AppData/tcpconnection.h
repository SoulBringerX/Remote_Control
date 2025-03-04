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

    Q_INVOKABLE bool sendPacket(const RD_Packet& packet); // 修改方法名

    Q_INVOKABLE bool receive(RD_Packet& packet);

    void close();

    static QString TCP_IP;
};

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    void startListening();

signals:
    void newConnection(QTcpSocket *socket);

private slots:
    void incomingConnection(); // 修改为无参数
    void clientDisconnected(); // 修改为无参数

private:
    QTcpServer *tcpServer;
};

#endif // TCPCONNECTION_H
