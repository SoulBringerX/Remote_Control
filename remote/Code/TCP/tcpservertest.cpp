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

    zpoller_t *poller = zpoller_new(responder_, NULL);
    if (!poller) {
        logger.print("RDP_Server", "Error: Failed to create zpoller");
        return;
    }

    while (m_running) {
        void *which = zpoller_wait(poller, 500);
        if (zpoller_terminated(poller)) {
            break;
        }

        if (which == responder_) {
            zmsg_t* request = zmsg_recv(responder_);
            if (!request) {
                logger.print("TCP_SERVER", "接受到无效消息");
            } else {
                zmsg_destroy(&request);
                logger.print("TCP_SERVER", "接受到客户传输应用列表请求");
                this->appListsend();
            }
        }

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

    zpoller_destroy(&poller);
}

// 停止方法
void tcpservertest::stop() {
    m_running = false;
    logger.print("RDP_Server", "Server stopping...");
}

// 发送应用列表
void tcpservertest::appListsend() {
    // 不再接收额外请求，直接构造响应
    logger.print("RDP_Server", "开始传输应用列表");
    InstalledSoftware pc_software;
    pc_software.refreshSoftwareList();

    zmsg_t* response = zmsg_new();

    foreach (const QVariant &entry, pc_software.softwareList()) {
        QVariantMap entryMap = entry.toMap();
        QString appName = entryMap["name"].toString();
        QString appIconPath = entryMap["mainExe"].toString();

        RD_Packet namePacket;
        memset(&namePacket, 0, sizeof(namePacket));
        namePacket.RD_Type = OperationCommandType::TransmitAppAlias;
        strncpy(namePacket.RD_APP_Name, appName.toUtf8().constData(), sizeof(namePacket.RD_APP_Name) - 1);
        zmsg_addmem(response, &namePacket, sizeof(namePacket));

        QFile iconFile(appIconPath);
        if (iconFile.open(QIODevice::ReadOnly)) {
            RD_Packet iconPacket;
            memset(&iconPacket, 0, sizeof(iconPacket));
            iconPacket.RD_Type = OperationCommandType::TransmitAppIconData;
            QByteArray iconData = iconFile.readAll();
            int copySize = qMin(iconData.size(), static_cast<int>(sizeof(iconPacket.RD_ImageBit)));
            memcpy(iconPacket.RD_ImageBit, iconData.constData(), copySize);
            zmsg_addmem(response, &iconPacket, sizeof(iconPacket));
        }

        RD_Packet endPacket;
        memset(&endPacket, 0, sizeof(endPacket));
        endPacket.RD_Type = OperationCommandType::TransmitOnceEnd;
        zmsg_addmem(response, &endPacket, sizeof(endPacket));
    }

    RD_Packet finalEnd;
    memset(&finalEnd, 0, sizeof(finalEnd));
    finalEnd.RD_Type = OperationCommandType::TransmitEnd;
    zmsg_addmem(response, &finalEnd, sizeof(finalEnd));

    zmsg_send(&response, responder_);
    logger.print("RDP_Server", "应用列表发送完成");
}

// 析构函数
tcpservertest::~tcpservertest() {
    if (responder_) {
        zsock_destroy(&responder_);
        responder_ = nullptr;
    }
}

#endif
