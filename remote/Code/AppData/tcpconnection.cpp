#include "tcpconnection.h"
#include <iostream>
#include <stdexcept>
#ifdef LINUX
#include <netdb.h>
#endif

QString tcpConnection::TCP_IP = "192.168.31.8"; // 默认值

tcpConnection::tcpConnection() : sockfd_(nullptr) {}

tcpConnection::~tcpConnection() {
    close();
}

bool tcpConnection::connectToServer(const QString &host) {
    if (sockfd_) {
        logger.print("CZMQ_TCP", "已有连接，先关闭");
        close();
    }

    sockfd_ = zsock_new_req(NULL);
    if (!sockfd_) {
        logger.print("CZMQ_TCP", "创建 CZMQ REQ socket 失败");
        return false;
    }

    tcpConnection::TCP_IP = host;
    QString ip_port = QString("tcp://") + host + QString(":5555");

    if (zsock_connect(sockfd_, ip_port.toUtf8().constData()) == -1) {
        logger.print("CZMQ_TCP", "目标IP：" + TCP_IP + " 连接失败");
        close();
        return false;
    }

    logger.print("CZMQ_TCP", "目标IP：" + TCP_IP + " 连接成功");
    return true;
}

bool tcpConnection::sendPacket(const RD_Packet &packet) {
    if (!sockfd_) {
        logger.print("CZMQ_TCP", "无效的 socket，发送失败");
        return false;
    }

    if (zsock_send(sockfd_, "b", &packet, sizeof(packet)) != 0) {
        logger.print("CZMQ_TCP", "目标发送数据包失败");
        return false;
    }

    logger.print("CZMQ_TCP", "目标发送数据包成功");
    return true;
}

bool tcpConnection::receive(RD_Packet &packet) {
    if (!sockfd_) {
        logger.print("CZMQ_TCP", "无效的 socket，接收失败");
        return false;
    }

    int rc = zsock_recv(sockfd_, "b", &packet, sizeof(packet));
    if (rc <= 0) {
        logger.print("CZMQ_TCP", "目标接收数据失败");
        return false;
    }

    logger.print("CZMQ_TCP", "成功接收数据包，大小：" + QString::number(sizeof(packet)));
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
        logger.print("CZMQ_TCP", "无效的 socket，无法请求应用列表");
        return appList;
    }

    // 发送请求：请求服务器返回应用列表
    RD_Packet requestPacket;
    requestPacket.RD_Type = OperationCommandType::TransmitAppAlias;
    memset(requestPacket.RD_APP_Name, 0, sizeof(requestPacket.RD_APP_Name));  // 确保没有额外字符
    if (zsock_send(sockfd_, "b", &requestPacket, sizeof(requestPacket)) != 0) {
        logger.print("CZMQ_TCP", "请求应用列表失败");
        return appList;
    }
    logger.print("CZMQ_TCP", "已发送应用列表请求");

    // 进入接收循环
    while (true) {
        RD_Packet packet;
        if (!receive(packet)) {
            logger.print("CZMQ_TCP", "接收应用信息失败");
            return appList;
        }

        if (packet.RD_Type == OperationCommandType::TransmitEnd) {
            logger.print("CZMQ_TCP", "接收应用信息结束");
            break;
        }

        if (packet.RD_Type == OperationCommandType::TransmitAppAlias) {
            QString appName(packet.RD_APP_Name);
            QString mainExePath(packet.RD_MainExePath);
            QString uninstallExePath(packet.RD_UninstallExePath);
            QByteArray iconData(packet.RD_ImageBit, sizeof(packet.RD_ImageBit));

            logger.print("CZMQ_TCP", "接收应用: " + appName);

            QVariantMap appInfo;
            appInfo["name"] = appName;
            appInfo["mainExe"] = mainExePath;
            appInfo["uninstallExe"] = uninstallExePath;
            appInfo["iconData"] = iconData;
            appList.append(appInfo);
        }
    }

    emit appListReceived(appList);
    return appList;
}
QVariantList tcpConnection::parseAppList() {
    return receiveAppList();
}
