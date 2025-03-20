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

    // 创建 REQ 类型 socket
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

    // 使用 "b" 格式直接发送整个结构体（二进制数据传输）
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

    // 构造请求数据包，并以二进制方式发送
    RD_Packet requestPacket;
    memset(&requestPacket, 0, sizeof(requestPacket));
    requestPacket.RD_Type = OperationCommandType::TransmitAppAlias;
    if (zsock_send(sockfd_, "b", &requestPacket, sizeof(requestPacket)) != 0) {
        emit connectionError("请求应用列表失败");
        return appList;
    }
    qDebug() << "已发送应用列表请求";

    // 使用 zmsg_recv() 接收整个多帧回复
    zmsg_t* reply = zmsg_recv(sockfd_);
    if (!reply) {
        emit connectionError("接收应用列表失败");
        return appList;
    }

    // 遍历回复中的所有帧
    zframe_t* frame;
    while ((frame = zmsg_pop(reply)) != NULL) {
        size_t size = zframe_size(frame);
        if (size != sizeof(RD_Packet)) {
            zframe_destroy(&frame);
            continue;  // 跳过无效的帧
        }
        RD_Packet packet;
        memcpy(&packet, zframe_data(frame), sizeof(RD_Packet));
        zframe_destroy(&frame);

        // 判断是否为结束标志
        if (packet.RD_Type == OperationCommandType::TransmitEnd) {
            qDebug() << "接收应用信息结束";
            break;
        }

        // 如果是应用信息包，组装应用数据
        if (packet.RD_Type == OperationCommandType::TransmitAppAlias) {
            QVariantMap appInfo;
            appInfo["name"] = QString::fromUtf8(packet.RD_APP_Name);
            appInfo["mainExe"] = QString::fromUtf8(packet.RD_MainExePath);
            appInfo["uninstallExe"] = QString::fromUtf8(packet.RD_UninstallExePath);
            appInfo["iconData"] = QByteArray(packet.RD_ImageBit, sizeof(packet.RD_ImageBit));
            appList.append(appInfo);
        }
    }
    zmsg_destroy(&reply);

    emit appListReceived(appList);
    return appList;
}


// 线程管理类实现
TcpThread::TcpThread(QObject *parent) : QThread(parent), tcpConn(nullptr) {}

TcpThread::~TcpThread() {
    quit();
    wait();
    delete tcpConn;
}

void TcpThread::run() {
    tcpConn = new tcpConnection();
    emit tcpReady(tcpConn);  // 通知外部 tcpConn 已经准备好
    exec();  // 启动事件循环，保持线程运行
}

tcpConnection* TcpThread::getTcpConnection() {
    return tcpConn;
}
