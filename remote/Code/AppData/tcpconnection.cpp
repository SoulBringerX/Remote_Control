#include "tcpconnection.h"
#include <iostream>
#include <stdexcept>
#ifdef LINUX
#include <netdb.h>
#endif

QString tcpConnection::TCP_IP = "192.168.31.8"; // 默认值

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
    // 创建一个 REQ 类型的 socket
    sockfd_ = zsock_new(ZMQ_REQ);
    assert(sockfd_);
    tcpConnection::TCP_IP = host;
    QString ip_port = QString("tcp://") + host + QString(":5555");
    // 连接到服务器
    if (zsock_connect(sockfd_, ip_port.toUtf8().constData()) == -1) {
        logger.print("CZMQ_TCP", "目标IP：" + TCP_IP + " 连接失败");
        return false;
    }
    logger.print("CZMQ_TCP", "目标IP：" + TCP_IP + " 连接成功");
    return true;
}

bool tcpConnection::sendPacket(const RD_Packet& packet) {
    // 发送数据包
    if (zsock_send(sockfd_, "i", packet.RD_Type) != 0) {
        logger.print("CZMQ_TCP", "目标发送消息成功");
    } else {
        logger.print("CZMQ_TCP", "目标发送消息失败");
        return false;
    }

    if (zsock_send(sockfd_, "s", packet.RD_APP_Name) != 0) {
        logger.print("CZMQ_TCP", "目标发送应用名称成功");
    } else {
        logger.print("CZMQ_TCP", "目标发送应用名称失败");
        return false;
    }

    if (zsock_send(sockfd_, "s", packet.RD_MainExePath) != 0) {
        logger.print("CZMQ_TCP", "目标发送主程序路径成功");
    } else {
        logger.print("CZMQ_TCP", "目标发送主程序路径失败");
        return false;
    }

    if (zsock_send(sockfd_, "s", packet.RD_UninstallExePath) != 0) {
        logger.print("CZMQ_TCP", "目标发送卸载程序路径成功");
    } else {
        logger.print("CZMQ_TCP", "目标发送卸载程序路径失败");
        return false;
    }

    if (zsock_send(sockfd_, "b", packet.RD_ImageBit, sizeof(packet.RD_ImageBit)) != 0) {
        logger.print("CZMQ_TCP", "目标发送图标数据成功");
    } else {
        logger.print("CZMQ_TCP", "目标发送图标数据失败");
        return false;
    }

    return true;
}
bool tcpConnection::receive(RD_Packet& packet) {
    // 接收数据包
    char buffer[sizeof(RD_Packet)];
    ssize_t received = zsock_recv(sockfd_, buffer);
    if (received <= 0) {
        logger.print("CZMQ_TCP", "目标接收成功");
        return false;
    }
    memcpy(&packet, buffer, received);

    // 接收主程序路径和卸载路径
    char mainExePath[512];
    char uninstallExePath[512];
    if (zsock_recv(sockfd_, "s", mainExePath) <= 0 ||
        zsock_recv(sockfd_, "s", uninstallExePath) <= 0) {
        logger.print("CZMQ_TCP", "接收主程序路径或卸载路径失败");
        return false;
    }

    // 接收图标数据
    if (zsock_recv(sockfd_, "b", packet.RD_ImageBit, sizeof(packet.RD_ImageBit)) <= 0) {
        logger.print("CZMQ_TCP", "接收图标数据失败");
        return false;
    }

    // 填充主程序路径和卸载路径
    strncpy(packet.RD_MainExePath, mainExePath, sizeof(packet.RD_MainExePath));
    strncpy(packet.RD_UninstallExePath, uninstallExePath, sizeof(packet.RD_UninstallExePath));

    return true;
}

void tcpConnection::close() {
    // 关闭 socket
    if (sockfd_ != nullptr) {
        zsock_destroy(&sockfd_);
        sockfd_ = nullptr;
    }
}

QVariantList tcpConnection::receiveAppList() {
    QVariantList appList;

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
            // 接收应用名称
            QString appName(packet.RD_APP_Name);
            logger.print("CZMQ_TCP", "接收应用名称: " + appName);

            // 接收主程序路径
            QString mainExePath(packet.RD_MainExePath);
            logger.print("CZMQ_TCP", "接收主程序路径: " + mainExePath);

            // 接收卸载路径
            QString uninstallExePath(packet.RD_UninstallExePath);
            logger.print("CZMQ_TCP", "接收卸载路径: " + uninstallExePath);

            // 接收应用图标数据
            QByteArray iconData(packet.RD_ImageBit, sizeof(packet.RD_ImageBit));
            logger.print("CZMQ_TCP", "接收应用图标数据大小: " + QString::number(iconData.size()));

            // 创建应用信息
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
