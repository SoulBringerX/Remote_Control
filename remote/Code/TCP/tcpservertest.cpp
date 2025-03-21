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
                zframe_t *frame = zmsg_first(request);
                int frameIdx = 0;

                while (frame) {
                    size_t size = zframe_size(frame);
                    const char *data = (const char*)zframe_data(frame);

                    // 确保数据至少有 RD_Packet 结构大小
                    if (size >= sizeof(RD_Packet)) {
                        const RD_Packet* packet = reinterpret_cast<const RD_Packet*>(data);

                        // 解析并输出 RD_Type
                        unsigned char commandType = static_cast<unsigned char>(packet->RD_Type);
                        logger.print("TCP_SERVER",
                                     QString("收到的消息类型: 0x%1 (%2)")
                                         .arg(commandType, 2, 16, QLatin1Char('0'))
                                         .arg(operationCommandTypeToString(static_cast<OperationCommandType>(commandType)))
                                     );

                        // 判断是否为 0x01 (TransmitAppAlias)
                        if (commandType == static_cast<unsigned char>(OperationCommandType::TransmitAppAlias)) {
                            logger.print("TCP_SERVER", "符合 0x01 (TransmitAppAlias)");
                            logger.print("TCP_SERVER","开始传输应用信息");
                            this->appListsend();
                        } else if(commandType == static_cast<unsigned char>(OperationCommandType::TransmitDeviceInformaiton)){
                            logger.print("TCP_SERVER", "符合 0x05 (OperationCommandType::TransmitDeviceInformaiton)");
                            logger.print("TCP_SERVER","开始传输硬件以及资源占用信息");
                            this->deviceInformationsend();
                        } else {
                            logger.print("TCP_SERVER", "⚠️ 不是 0x01，实际收到: " + QString::number(commandType, 16));
                        }
                    } else {
                        logger.print("TCP_SERVER", "⚠️ 收到的数据包长度不符合 RD_Packet 预期大小");
                    }

                    frame = zmsg_next(request);
                    frameIdx++;
                }

                zmsg_destroy(&request);
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

// 获取到设备的相关硬件、资源占用信息并发送到客户端
void tcpservertest::deviceInformationsend() {
    logger.print("RDP_Server", "📡 开始传输设备信息...");

    // 获取 CPU 信息
    QProcess cpuProcess;
    cpuProcess.start("wmic cpu get name, numberofcores");
    cpuProcess.waitForFinished();
    QString cpuOutput = cpuProcess.readAllStandardOutput();
    QStringList cpuLines = cpuOutput.split("\n", Qt::SkipEmptyParts);
    QString cpuModel = (cpuLines.size() > 1) ? cpuLines[1].split(QRegularExpression("\\s+"), Qt::SkipEmptyParts)[0].trimmed() : "Unknown";
    int cpuCores = (cpuLines.size() > 1) ? cpuLines[1].split(QRegularExpression("\\s+"), Qt::SkipEmptyParts)[1].trimmed().toInt() : 0;

    // 获取内存信息
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    quint64 totalMemory = 0, availableMemory = 0, usedMemory = 0;
    if (GlobalMemoryStatusEx(&memoryStatus)) {
        totalMemory = memoryStatus.ullTotalPhys / (1024 * 1024); // 转换为 MB
        availableMemory = memoryStatus.ullAvailPhys / (1024 * 1024);
        usedMemory = totalMemory - availableMemory;
    } else {
        logger.print("RDP_Server", "❌ 获取内存信息失败");
    }

    // 获取磁盘信息
    QStorageInfo storage = QStorageInfo::root();
    quint64 totalDisk = storage.bytesTotal() / (1024 * 1024 * 1024); // GB
    quint64 usedDisk = totalDisk - (storage.bytesFree() / (1024 * 1024 * 1024));

    // 获取 CPU 使用率
    QProcess cpuUsageProcess;
    cpuUsageProcess.start("wmic cpu get loadpercentage");
    cpuUsageProcess.waitForFinished();
    QString cpuUsageOutput = cpuUsageProcess.readAllStandardOutput();
    double cpuUsage = (cpuUsageOutput.split("\n", Qt::SkipEmptyParts).size() > 1)
                          ? cpuUsageOutput.split("\n")[1].trimmed().toDouble()
                          : 0.0;

    // **打印设备信息**
    logger.print("RDP_Server", "✅ 获取到的设备信息如下：");
    logger.print("RDP_Server", QString("🖥️ CPU 型号: %1").arg(cpuModel));
    logger.print("RDP_Server", QString("🧩 CPU 核心数: %1").arg(cpuCores));
    logger.print("RDP_Server", QString("⚡ CPU 使用率: %1%").arg(cpuUsage));
    logger.print("RDP_Server", QString("💾 总内存: %1 MB").arg(totalMemory));
    logger.print("RDP_Server", QString("📊 已用内存: %1 MB").arg(usedMemory));
    logger.print("RDP_Server", QString("🗄️ 总磁盘大小: %1 GB").arg(totalDisk));
    logger.print("RDP_Server", QString("📂 已用磁盘大小: %1 GB").arg(usedDisk));

    // **构造设备信息结构体**
    DeviceInfo info;
    memset(&info, 0, sizeof(DeviceInfo));
    strncpy(info.cpuModel, cpuModel.toUtf8().constData(), sizeof(info.cpuModel) - 1);
    info.cpuCores = cpuCores;
    info.cpuUsage = cpuUsage;
    info.totalMemory = totalMemory;
    info.usedMemory = usedMemory;
    info.totalDisk = totalDisk;
    info.usedDisk = usedDisk;

    // **创建 ZeroMQ 消息**
    zmsg_t* response = zmsg_new();
    zframe_t* dataFrame = zframe_new(&info, sizeof(info));
    zmsg_append(response, &dataFrame);

    // 发送数据
    zmsg_send(&response, responder_);
    logger.print("RDP_Server", "📤 设备信息发送完成 ✅");
}



// 析构函数
tcpservertest::~tcpservertest() {
    if (responder_) {
        zsock_destroy(&responder_);
        responder_ = nullptr;
    }
}

#endif
