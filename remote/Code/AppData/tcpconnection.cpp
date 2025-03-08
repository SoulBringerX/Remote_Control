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

bool tcpConnection::sendPacket(const QVariantMap &packetMap) {
    RD_Packet packet;
    // 清零 RD_Packet 内存，避免残留数据
    memset(&packet, 0, sizeof(RD_Packet));

    // 根据 packetMap 中的键设置对应字段
    if (packetMap.contains("RD_Type"))
        packet.RD_Type = static_cast<OperationCommandType>(packetMap.value("RD_Type").toInt());
    if (packetMap.contains("RD_APP_Name")) {
        QString appName = packetMap.value("RD_APP_Name").toString();
        strncpy(packet.RD_APP_Name, appName.toUtf8().constData(), sizeof(packet.RD_APP_Name) - 1);
    }
    if (packetMap.contains("RD_MainExePath")) {
        QString mainExePath = packetMap.value("RD_MainExePath").toString();
        strncpy(packet.RD_MainExePath, mainExePath.toUtf8().constData(), sizeof(packet.RD_MainExePath) - 1);
    }
    if (packetMap.contains("RD_UninstallExePath")) {
        QString uninstallExePath = packetMap.value("RD_UninstallExePath").toString();
        strncpy(packet.RD_UninstallExePath, uninstallExePath.toUtf8().constData(), sizeof(packet.RD_UninstallExePath) - 1);
    }
    if (packetMap.contains("RD_ImageBit")) {
        QByteArray iconData = packetMap.value("RD_ImageBit").toByteArray();
        int dataSize = qMin(iconData.size(), static_cast<int>(sizeof(packet.RD_ImageBit)));
        memcpy(packet.RD_ImageBit, iconData.constData(), dataSize);
    }

    // 发送数据包，转换 RD_Packet 指针为 const char*
    if (zsock_send(sockfd_, reinterpret_cast<const char*>(&packet), sizeof(packet), 0) == 0) {
        logger.print("CZMQ_TCP", "发送请求包成功");
        return true;
    } else {
        logger.print("CZMQ_TCP", "发送请求包失败");
        return false;
    }
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
