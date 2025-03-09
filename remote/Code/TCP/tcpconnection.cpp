#include "tcpconnection.h"
#include <iostream>
#include <stdexcept>
#include <QDebug>

QString tcpConnection::TCP_IP = "192.168.31.8";

tcpConnection::tcpConnection(QObject *parent) : QObject(parent), sockfd_(nullptr) {}

tcpConnection::~tcpConnection() {
    close();
}

bool tcpConnection::connectToServer(const QString &host) {
    if (sockfd_) {
        close();
    }

    sockfd_ = zsock_new_req(NULL);
    if (!sockfd_) {
        emit connectionError("创建 ZMQ REQ socket 失败");
        return false;
    }

    TCP_IP = host;
    QString ip_port = QString("tcp://") + host + ":5555";

    if (zsock_connect(sockfd_, ip_port.toUtf8().constData()) == -1) {
        emit connectionError("目标IP：" + TCP_IP + " 连接失败");
        close();
        return false;
    }

    qDebug() << "目标IP：" << TCP_IP << " 连接成功";
    return true;
}

bool tcpConnection::sendPacket(const RD_Packet &packet) {
    if (!sockfd_) {
        emit connectionError("无效的 socket，发送失败");
        return false;
    }

    if (zsock_send(sockfd_, "b", &packet, sizeof(packet)) != 0) {
        emit connectionError("目标发送数据包失败");
        return false;
    }

    qDebug() << "目标发送数据包成功";
    return true;
}

bool tcpConnection::receive(RD_Packet &packet) {
    if (!sockfd_) {
        emit connectionError("无效的 socket，接收失败");
        return false;
    }

    int rc = zsock_recv(sockfd_, "b", &packet, sizeof(packet));
    if (rc <= 0) {
        emit connectionError("目标接收数据失败");
        return false;
    }

    qDebug() << "成功接收数据包，大小：" << sizeof(packet);
    return true;
}

void tcpConnection::close() {
    if (sockfd_) {
        zsock_destroy(&sockfd_);
        sockfd_ = nullptr;
    }
}

QVariantList tcpConnection::receiveAppList() {
    QVariantList appList;

    if (!sockfd_) {
        emit connectionError("无效的 socket，无法请求应用列表");
        return appList;
    }

    // 发送请求
    RD_Packet requestPacket;
    requestPacket.RD_Type = OperationCommandType::TransmitAppAlias;
    memset(requestPacket.RD_APP_Name, 0, sizeof(requestPacket.RD_APP_Name));

    if (zsock_send(sockfd_, "b", &requestPacket, sizeof(requestPacket)) != 0) {
        emit connectionError("请求应用列表失败");
        return appList;
    }
    qDebug() << "已发送应用列表请求";

    // 接收数据
    while (true) {
        RD_Packet packet;
        if (!receive(packet)) {
            emit connectionError("接收应用信息失败");
            return appList;
        }

        if (packet.RD_Type == OperationCommandType::TransmitEnd) {
            qDebug() << "接收应用信息结束";
            break;
        }

        if (packet.RD_Type == OperationCommandType::TransmitAppAlias) {
            QVariantMap appInfo;
            appInfo["name"] = QString(packet.RD_APP_Name);
            appInfo["mainExe"] = QString(packet.RD_MainExePath);
            appInfo["uninstallExe"] = QString(packet.RD_UninstallExePath);
            appInfo["iconData"] = QByteArray(packet.RD_ImageBit, sizeof(packet.RD_ImageBit));

            appList.append(appInfo);
        }
    }

    emit appListReceived(appList);
    return appList;
}

// 线程管理类
TcpThread::TcpThread(QObject *parent) : QThread(parent), tcpConn(nullptr) {}

TcpThread::~TcpThread() {
    quit();
    wait();
    delete tcpConn;
}

void TcpThread::run() {
    tcpConn = new tcpConnection();
    emit tcpReady(tcpConn);  // 发出信号，通知外部 tcpConn 已准备好
    exec();  // 启动事件循环，保持线程运行
}

tcpConnection* TcpThread::getTcpConnection() {
    return tcpConn;
}
