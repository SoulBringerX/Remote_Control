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
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> // 添加 netdb.h
#endif

#define SERVER_PORT 12345
#pragma pack(push, 1)
struct RD_Packet {
    char RD_IP[16];
    char RD_Username[33];
    char RD_Password[33];
    unsigned char RD_Type;
    char RD_ImageBit[1024];
    char RD_APP_Name[33];
    char RD_Command_Name[65];

    RD_Packet() {
        memset(this, 0, sizeof(RD_Packet));
    }
};
#pragma pack(pop)

class tcpConnection : public QObject {
    Q_OBJECT
private:
    int sockfd_;

public:
    tcpConnection();
    ~tcpConnection();

    bool connect(const std::string& host, const std::string& port);

    bool sendPacket(const RD_Packet& packet); // 修改方法名

    bool receive(RD_Packet& packet);

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
    void incomingConnection(); // 修改为无参数
    void clientDisconnected(); // 修改为无参数

private:
    QTcpServer *tcpServer;
};

#endif // TCPCONNECTION_H
