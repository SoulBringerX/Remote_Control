#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <string>
#include <cstring>
#include <vector>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include "./installedSoftware.h"
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


#define SERVER_PORT 12345
#pragma pack(push, 1)  // 禁用结构体对齐，确保数据包格式正确
struct RD_Packet {
    char RD_IP[16];               // 远端设备 IP（IPv4 地址）
    char RD_Username[33];         // 远端设备用户名
    char RD_Password[33];         // 远端设备密码
    unsigned char RD_Type;        // 指令类型
    char RD_ImageBit[1024];       // 应用图标数据
    char RD_APP_Name[33];         // 应用别名
    char RD_Command_Name[65];     // 应用程序名

    RD_Packet() {
        memset(this, 0, sizeof(RD_Packet));
    }
};
#pragma pack(pop)

class tcpConnection {
private:
    int sockfd_;       // 套接字描述符

public:
    tcpConnection();
    ~tcpConnection();

    // 连接到远程设备
    bool connect(const std::string& host, const std::string& port);

    // 发送数据包
    bool send(const RD_Packet& packet);

    // 接收数据包
    bool receive(RD_Packet& packet);

    // 关闭连接
    void close();
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
    void incomingConnection(qintptr socketDescriptor);
    void clientDisconnected();
};
#endif // TCPCONNECTION_H
