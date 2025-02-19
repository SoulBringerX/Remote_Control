#include "tcpconnection.h"
#include <iostream>
#include <stdexcept>
#include <netdb.h>

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
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    addrinfo hints{};
    addrinfo *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int result = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << result << std::endl;
        close();
        freeaddrinfo(res);
        return false;
    }

    // 使用 ::connect 并将参数类型转换为 sockaddr*
    if (::connect(sockfd_, reinterpret_cast<sockaddr*>(res->ai_addr), res->ai_addrlen) == -1) {
        std::cerr << "Connection failed" << std::endl;
        close();
        freeaddrinfo(res);
        return false;
    }

    freeaddrinfo(res);
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
    if (!tcpServer->listen(QHostAddress::Any, 12345)) {
        qDebug() << "Failed to start server:" << tcpServer->errorString();
        return;
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &TcpServer::incomingConnection);
    qDebug() << "Server started on port 12345";
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
