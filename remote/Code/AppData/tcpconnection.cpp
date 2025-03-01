#include "tcpconnection.h"
#include <iostream>
#include <stdexcept>
#ifdef LINUX
#include <netdb.h>
#endif
QString tcpConnection::TCP_IP = "127.0.0.1"; // 默认值
tcpConnection::tcpConnection() : sockfd_(nullptr) {
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
    sockfd_ = zsock_new(ZMQ_REQ);
    assert(sockfd_);
    tcpConnection::TCP_IP = host;
    QString ip_port = QString("tcp://") + QString(TCP_IP) + QString(":5555");
    if (zsock_connect(sockfd_, ip_port.toUtf8()) == -1) {
        logger.print("CZNQ_TCP", "目标IP：" + TCP_IP + " 连接失败");
        return false;
    }
    logger.print("CZNQ_TCP", "目标IP：" + TCP_IP + " 连接成功");
    return true;
}

bool tcpConnection::sendPacket(const RD_Packet& packet) {
    if (zsock_send(sockfd_, "i", packet.RD_Type) != 0) {
        logger.print("CZNQ_TCP", "目标发送消息成功");
    } else {
        logger.print("CZNQ_TCP", "目标发送消息失败");
        return false;
    }
    return true;
}

bool tcpConnection::receive(RD_Packet& packet) {
    char buffer[sizeof(RD_Packet)];
    ssize_t received = zsock_recv(sockfd_, buffer);
    if (received <= 0) {
        logger.print("CZNQ_TCP", "目标接收成功");
        return false;
    }
    memcpy(&packet, buffer, received);
    return true;
}

void tcpConnection::close() {
    if (sockfd_ != nullptr) {
        zsock_destroy(&sockfd_);
        sockfd_ = nullptr;
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
