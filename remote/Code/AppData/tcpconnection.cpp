#include "tcpconnection.h"
#include <iostream>
#include <stdexcept>
#include <netdb.h>
QString tcpConnection::TCP_IP = "127.0.0.1"; // 默认值
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

bool tcpConnection::connect(const QString host) {
    // 此处使用CZMQ进行TCP连接简化

    // 创建请求套接字
    zsock_t *requester = zsock_new(ZMQ_REQ);
    assert(requester);
    tcpConnection::TCP_IP = host;
    QString ip_port = "";
    ip_port = QString("tcp://") + QString(TCP_IP) + QString(":5555");
    // 连接到服务端
    sockfd_ = zsock_connect(requester, ip_port.toUtf8());
    if (sockfd_ == -1) {
        qDebug()<<TCP_IP<<"连接失败";
        return false;
    }
    qDebug()<<TCP_IP<<"连接成功";
    return true;
}

bool tcpConnection::sendPacket(const RD_Packet& packet) { // 修改方法名
    const void* data = &packet;
    size_t size = sizeof(RD_Packet);

    ssize_t sent = ::send(sockfd_, static_cast<const char*>(data), size, 0); // 使用 ::send 避免冲突
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
    : QObject(parent), tcpServer(nullptr)
{
}

void TcpServer::startListening()
{
    tcpServer = new QTcpServer(this);
    // 监听所有的IP地址且端口为5555
    if (!tcpServer->listen(QHostAddress::Any, 5555)) {
        qDebug() << "Failed to start server:" << tcpServer->errorString();
        return;
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &TcpServer::incomingConnection);
    qDebug() << "Server started on port 5555";
}

void TcpServer::incomingConnection()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    if (socket) {
        qDebug() << "New connection from" << socket->peerAddress().toString();
        emit newConnection(socket);

        connect(socket, &QTcpSocket::disconnected, this, &TcpServer::clientDisconnected);
    }
}

void TcpServer::clientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket) {
        qDebug() << "Client disconnected" << socket->peerAddress().toString();
        socket->deleteLater();
    }
}
