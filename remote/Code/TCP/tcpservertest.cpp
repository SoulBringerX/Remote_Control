#ifdef WIN32
// tcpservertest.cpp
#include "tcpservertest.h"
#include "../AppData/installedsoftware.h"
#include <QDebug>
#include <QElapsedTimer>
#include <czmq.h>  // 使用 CZMQ

// 构造函数：使用 CZMQ 创建 REP 套接字并绑定到 tcp://*:5555
tcpservertest::tcpservertest() : m_running(true) {
    responder_ = zsock_new_rep("tcp://*:5555");
    if (!responder_) {
        logger.print("RDP_Server", "Error: Failed to create or bind REP socket (CZMQ)");
        m_running = false;
        return;
    }
    logger.print("RDP_Server", "StartListening >>>>>>>>");
}

// 核心执行逻辑：基于 CZMQ 的非阻塞接收
void tcpservertest::exec() {
    if (!responder_) {
        logger.print("RDP_Server", "Error: No valid ZMQ responder socket.");
        return;
    }

    QElapsedTimer timer;
    timer.start();
    int lastStatusTime = 0;  // 记录上一次状态输出的秒数

    // 使用 CZMQ 的 zpoller 进行事件轮询（代替 zsock_wait）
    zpoller_t *poller = zpoller_new(responder_, NULL);
    if (!poller) {
        logger.print("RDP_Server", "Error: Failed to create zpoller");
        return;
    }

    while (m_running) {
        void *which = zpoller_wait(poller, 500);  // 500ms 超时等待
        if (zpoller_terminated(poller)) {
            break;  // 轮询器被销毁，退出循环
        }

        if (which == responder_) {
            // 套接字有数据可读
            int rc = zsock_recv(responder_, "b", &recvPacket_, sizeof(recvPacket_));
            if (rc < 0) {
                logger.print("RDP_Server", "Error receiving message");
                continue;
            }
            logger.print("RDP_Server", "recvPacket数据包大小：" + QString::number(sizeof(recvPacket_)));

            if (recvPacket_.RD_Type == OperationCommandType::TransmitAppAlias) {
                appListsend();
            }
            zsock_send(responder_, "s", "WAIT");
        }

        // 计时逻辑
        int elapsedSec = timer.elapsed() / 1000;
        if (elapsedSec - lastStatusTime >= 5) {
            logger.print("RDP_Server", QString("当前等待客户端连接，已等待 %1 秒").arg(elapsedSec));
            lastStatusTime = elapsedSec;
        }
        if (elapsedSec >= 300) {
            logger.print("RDP_Server", "等待客户端连接超时300秒，自动关闭线程");
            break;
        }
    }

    zpoller_destroy(&poller);  // 释放轮询器
}

// 新增停止方法
void tcpservertest::stop() {
    m_running = false;
    logger.print("RDP_Server", "Server stopping...");
}

// 传输应用别名和图标数据
void tcpservertest::appListsend() {
    logger.print("RDP_Server", "传输应用别名");
    InstalledSoftware *pc_software = new InstalledSoftware();
    pc_software->refreshSoftwareList();

    foreach (const QVariant &entry, pc_software->softwareList()) {
        QVariantMap entryMap = entry.toMap();
        QString appName = entryMap["name"].toString();
        QString appIconPath = entryMap["mainExe"].toString();

        // 发送应用名称
        RD_Packet namePacket;
        namePacket.RD_Type = OperationCommandType::TransmitAppAlias;
        strncpy(namePacket.RD_APP_Name, appName.toStdString().c_str(), sizeof(namePacket.RD_APP_Name) - 1);
        zsock_send(responder_, "b", &namePacket, sizeof(namePacket));

        // 发送应用图标数据
        RD_Packet iconPacket;
        iconPacket.RD_Type = OperationCommandType::TransmitAppIconData;
        QFile iconFile(appIconPath);
        if (iconFile.open(QIODevice::ReadOnly)) {
            QByteArray iconData = iconFile.readAll();
            int dataSize = qMin(iconData.size(), static_cast<int>(sizeof(iconPacket.RD_ImageBit)));
            memcpy(iconPacket.RD_ImageBit, iconData.constData(), dataSize);
            zsock_send(responder_, "b", &iconPacket, sizeof(iconPacket));
        }

        // 发送结束标志
        RD_Packet endPacket;
        endPacket.RD_Type = OperationCommandType::TransmitEnd;
        zsock_send(responder_, "b", &endPacket, sizeof(endPacket));
    }
}

// 析构函数：销毁 CZMQ 套接字
tcpservertest::~tcpservertest() {
    if (responder_) {
        zsock_destroy(&responder_);  // 确保传递正确的类型
        responder_ = nullptr;  // 避免悬空指针
    }
}

#endif
