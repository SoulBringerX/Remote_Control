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
            char *str = zstr_recv(responder_);
            if (str == nullptr) // 修复 null 为 nullptr
            {
                logger.print("TCP_SERVER", "接受到无效消息");
            }
            else if (strcmp(str, operationCommandTypeToString(OperationCommandType::TransmitAppAlias)) == 0) // 修复比较逻辑
            {
                logger.print("TCP_SERVER", "接受到客户传输应用列表请求");
                this->appListsend();
            }
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

void tcpservertest::appListsend() {
    logger.print("RDP_Server", "传输应用别名");
    InstalledSoftware *pc_software = new InstalledSoftware();
    pc_software->refreshSoftwareList();

    foreach (const QVariant &entry, pc_software->softwareList()) {
        QVariantMap entryMap = entry.toMap();
        QString appName = entryMap["name"].toString();
        QString appIconPath = entryMap["mainExe"].toString();

        // 打印应用信息
        logger.print("RDP_Server", QString("应用名称: %1").arg(appName));
        logger.print("RDP_Server", QString("主程序路径: %1").arg(appIconPath));

        // 发送应用名称
        RD_Packet namePacket;
        namePacket.RD_Type = OperationCommandType::TransmitAppAlias;
        strncpy(namePacket.RD_APP_Name, appName.toStdString().c_str(), sizeof(namePacket.RD_APP_Name) - 1);

        // 将 RD_Packet 转换为字符串并发送
        char buffer1[sizeof(namePacket) + 1]; // +1 用于 null 结尾
        memcpy(buffer1, &namePacket, sizeof(namePacket));
        buffer1[sizeof(namePacket)] = '\0';
        zstr_send(responder_, buffer1);

        // 打印发送的应用名称数据包内容
        logger.print("RDP_Server", "发送应用名称数据包");
        logger.print("RDP_Server", QString("RD_IP: %1").arg(namePacket.RD_IP));
        logger.print("RDP_Server", QString("RD_Username: %1").arg(namePacket.RD_Username));
        logger.print("RDP_Server", QString("RD_Password: %1").arg(namePacket.RD_Password));
        logger.print("RDP_Server", QString("RD_Type: %1").arg(static_cast<int>(namePacket.RD_Type)));
        logger.print("RDP_Server", QString("RD_APP_Name: %1").arg(namePacket.RD_APP_Name));
        logger.print("RDP_Server", QString("RD_Command_Name: %1").arg(namePacket.RD_Command_Name));
        logger.print("RDP_Server", QString("RD_MainExePath: %1").arg(namePacket.RD_MainExePath));
        logger.print("RDP_Server", QString("RD_UninstallExePath: %1").arg(namePacket.RD_UninstallExePath));

        // 发送应用图标数据
        RD_Packet iconPacket;
        iconPacket.RD_Type = OperationCommandType::TransmitAppIconData;
        QFile iconFile(appIconPath);
        if (iconFile.open(QIODevice::ReadOnly)) {
            QByteArray iconData = iconFile.readAll();
            int dataSize = qMin(iconData.size(), static_cast<int>(sizeof(iconPacket.RD_ImageBit)));
            memcpy(iconPacket.RD_ImageBit, iconData.constData(), dataSize);

            // 将 RD_Packet 转换为字符串并发送
            char buffer2[sizeof(iconPacket) + 1]; // +1 用于 null 结尾
            memcpy(buffer2, &iconPacket, sizeof(iconPacket));
            buffer2[sizeof(iconPacket)] = '\0';
            zstr_send(responder_, buffer2);

            // 打印发送的应用图标数据包内容
            logger.print("RDP_Server", "发送应用图标数据包");
            logger.print("RDP_Server", QString("RD_IP: %1").arg(iconPacket.RD_IP));
            logger.print("RDP_Server", QString("RD_Username: %1").arg(iconPacket.RD_Username));
            logger.print("RDP_Server", QString("RD_Password: %1").arg(iconPacket.RD_Password));
            logger.print("RDP_Server", QString("RD_Type: %1").arg(static_cast<int>(iconPacket.RD_Type)));
            logger.print("RDP_Server", QString("RD_ImageBit 大小: %1").arg(dataSize));
            logger.print("RDP_Server", QString("RD_APP_Name: %1").arg(iconPacket.RD_APP_Name));
            logger.print("RDP_Server", QString("RD_Command_Name: %1").arg(iconPacket.RD_Command_Name));
            logger.print("RDP_Server", QString("RD_MainExePath: %1").arg(iconPacket.RD_MainExePath));
            logger.print("RDP_Server", QString("RD_UninstallExePath: %1").arg(iconPacket.RD_UninstallExePath));
        }

        // 发送结束标志
        RD_Packet endPacket;
        endPacket.RD_Type = OperationCommandType::TransmitEnd;

        // 将 RD_Packet 转换为字符串并发送
        char buffer3[sizeof(endPacket) + 1]; // +1 用于 null 结尾
        memcpy(buffer3, &endPacket, sizeof(endPacket));
        buffer3[sizeof(endPacket)] = '\0';
        zstr_send(responder_, buffer3);

        // 打印发送的结束标志数据包内容
        logger.print("RDP_Server", "发送结束标志数据包");
        logger.print("RDP_Server", QString("RD_IP: %1").arg(endPacket.RD_IP));
        logger.print("RDP_Server", QString("RD_Username: %1").arg(endPacket.RD_Username));
        logger.print("RDP_Server", QString("RD_Password: %1").arg(endPacket.RD_Password));
        logger.print("RDP_Server", QString("RD_Type: %1").arg(static_cast<int>(endPacket.RD_Type)));
        logger.print("RDP_Server", QString("RD_APP_Name: %1").arg(endPacket.RD_APP_Name));
        logger.print("RDP_Server", QString("RD_Command_Name: %1").arg(endPacket.RD_Command_Name));
        logger.print("RDP_Server", QString("RD_MainExePath: %1").arg(endPacket.RD_MainExePath));
        logger.print("RDP_Server", QString("RD_UninstallExePath: %1").arg(endPacket.RD_UninstallExePath));

        Sleep(1000);
    }

    // 发送最终的结束标志
    RD_Packet finalEndPacket;
    finalEndPacket.RD_Type = OperationCommandType::TransmitEnd;

    // 将 RD_Packet 转换为字符串并发送
    char buffer4[sizeof(finalEndPacket) + 1]; // +1 用于 null 结尾
    memcpy(buffer4, &finalEndPacket, sizeof(finalEndPacket));
    buffer4[sizeof(finalEndPacket)] = '\0';
    zstr_send(responder_, buffer4);

    // 打印发送的最终结束标志数据包内容
    logger.print("RDP_Server", "发送最终结束标志数据包");
    logger.print("RDP_Server", QString("RD_IP: %1").arg(finalEndPacket.RD_IP));
    logger.print("RDP_Server", QString("RD_Username: %1").arg(finalEndPacket.RD_Username));
    logger.print("RDP_Server", QString("RD_Password: %1").arg(finalEndPacket.RD_Password));
    logger.print("RDP_Server", QString("RD_Type: %1").arg(static_cast<int>(finalEndPacket.RD_Type)));
    logger.print("RDP_Server", QString("RD_APP_Name: %1").arg(finalEndPacket.RD_APP_Name));
    logger.print("RDP_Server", QString("RD_Command_Name: %1").arg(finalEndPacket.RD_Command_Name));
    logger.print("RDP_Server", QString("RD_MainExePath: %1").arg(finalEndPacket.RD_MainExePath));
    logger.print("RDP_Server", QString("RD_UninstallExePath: %1").arg(finalEndPacket.RD_UninstallExePath));
}
// 析构函数：销毁 CZMQ 套接字
tcpservertest::~tcpservertest() {
    if (responder_) {
        zsock_destroy(&responder_);  // 确保传递正确的类型
        responder_ = nullptr;  // 避免悬空指针
    }
}

#endif
