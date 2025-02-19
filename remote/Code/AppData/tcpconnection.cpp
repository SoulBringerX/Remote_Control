#include "tcpConnection.h"
#include <iostream>
#include <stdexcept>

tcpConnection::tcpConnection() : sockfd_(-1) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
#endif
}

tcpConnection::~tcpConnection() {
    close();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool tcpConnection::connect(const std::string& host, const std::string& port) {
    // 创建套接字
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    // 设置地址信息
    addrinfo hints{}, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int result = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << result << std::endl;
        close();
        return false;
    }

    // 连接到服务器
    if (connect(sockfd_, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Connection failed" << std::endl;
        close();
        freeaddrinfo(res);
        return false;
    }

    freeaddrinfo(res);
    return true;
}

bool tcpConnection::send(const RD_Packet& packet) {
    const void* data = &packet;
    size_t size = sizeof(RD_Packet);

    ssize_t sent = send(sockfd_, static_cast<const char*>(data), size, 0);
    if (sent == -1 || static_cast<size_t>(sent) != size) {
        std::cerr << "Send failed" << std::endl;
        return false;
    }

    return true;
}

bool tcpConnection::receive(RD_Packet& packet) {
    char buffer[sizeof(RD_Packet)];
    ssize_t received = recv(sockfd_, buffer, sizeof(buffer), 0);
    if (received <= 0) {
        std::cerr << "Receive failed" << std::endl;
        return false;
    }

    memcpy(&packet, buffer, received);
    return true;
}

void tcpConnection::close() {
    if (sockfd_ != -1) {
#ifdef _WIN32
        closesocket(sockfd_);
#else
        ::close(sockfd_);
#endif
        sockfd_ = -1;
    }
}


TcpServer::TcpServer(QObject *parent)
    : QObject(parent)
{
}

void TcpServer::startListening()
{
    auto *server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, SERVER_PORT)) {
        qDebug() << "无法启动服务器:" << server->errorString();
        return;
    }

    connect(server, &QTcpServer::newConnection, this, [=]() {
        QTcpSocket *client = server->nextPendingConnection();
        if (client) {
            // 发出信号通知有新连接
            emit newConnection(client);

            // 连接关闭信号
            connect(client, &QTcpSocket::disconnected, this, &TcpServer::clientDisconnected);
        }
    });

    qDebug() << "服务器启动成功，监听端口:" << server->serverPort();
}
