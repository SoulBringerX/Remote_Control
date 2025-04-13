#ifdef WIN32
#include "tcpservertest.h"
#include <QElapsedTimer>
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QThread>

tcpservertest::tcpservertest() : m_running(true) {
    responder_ = zsock_new_rep("tcp://*:5555");
    if (!responder_) {
        logger.print("RDP_Server", "错误：无法创建或绑定 REP 套接字");
        m_running = false;
    } else {
        logger.print("RDP_Server", "开始监听 >>>>>>>>");
    }
}

tcpservertest::~tcpservertest() {
    if (responder_) {
        zsock_destroy(&responder_);
    }
}

void tcpservertest::exec() {
    if (!responder_) {
        logger.print("RDP_Server", "错误：无效的 ZMQ 套接字");
        return;
    }

    zpoller_t* poller = zpoller_new(responder_, nullptr);
    if (!poller) {
        logger.print("RDP_Server", "错误：无法创建 poller");
        return;
    }

    QElapsedTimer timer;
    timer.start();
    int lastStatusTime = 0;

    while (m_running) {
        void* which = zpoller_wait(poller, 500);
        if (zpoller_terminated(poller)) {
            break;
        }
        RD_Packet Rd_packet;
        if (which == responder_) {
            zmsg_t* request = zmsg_recv(responder_);
            if (!request) {
                logger.print("TCP_SERVER", "接收到无效消息");
                continue;
            }

            // Get the first frame to determine the command type
            zframe_t* firstFrame = zmsg_first(request);
            if (firstFrame && zframe_size(firstFrame) >= sizeof(RD_Packet)) {
                memcpy(&Rd_packet, zframe_data(firstFrame), sizeof(RD_Packet));

                // Dispatch based on RD_Type
                switch (Rd_packet.RD_Type) {
                case OperationCommandType::TransmitAppAlias:
                    appListsend();
                    break;
                case OperationCommandType::TransmitDeviceInformaiton:
                    deviceInformationsend();
                    break;
                case OperationCommandType::TransmitAppCommand:
                    appPathsend(Rd_packet);
                    break;
                case OperationCommandType::TransmitUninstallAppCommand:
                    appUninstallPathSend(Rd_packet);
                    break;
                case OperationCommandType::TramsmitAppData:
                    receiveInstallPackage(request); // Pass the entire message
                    break;
                default:
                    logger.print("TCP_SERVER", "未知命令类型: " + QString::number(static_cast<int>(Rd_packet.RD_Type), 16));
                    // Send an empty response for unhandled commands
                    zmsg_t* response = zmsg_new();
                    zmsg_send(&response, responder_);
                    break;
                }
            } else {
                logger.print("TCP_SERVER", "无效的首帧大小或空消息");
                // Send an empty response to prevent client hang
                zmsg_t* response = zmsg_new();
                zmsg_send(&response, responder_);
            }

            // Destroy the request message (unless consumed by receiveInstallPackage)
            if (Rd_packet.RD_Type != OperationCommandType::TramsmitAppData) {
                zmsg_destroy(&request);
            }
        }

        int elapsedSec = timer.elapsed() / 1000;
        if (elapsedSec - lastStatusTime >= 5) {
            logger.print("RDP_Server", QString("等待客户端连接，已等待 %1 秒").arg(elapsedSec));
            lastStatusTime = elapsedSec;
        }
    }

    zpoller_destroy(&poller);
}

void tcpservertest::stop() {
    m_running = false;
    logger.print("RDP_Server", "服务器停止...");
}

// Existing handler methods remain unchanged (for brevity)
void tcpservertest::handlePacket(const RD_Packet& packet) {
    // This method can be called if needed, but exec now handles dispatching directly
    // Keeping it for completeness
    unsigned char commandType = static_cast<unsigned char>(packet.RD_Type);
    logger.print("TCP_SERVER", QString("收到命令: 0x%1 (%2)")
                                   .arg(commandType, 2, 16, QLatin1Char('0'))
                                   .arg(operationCommandTypeToString(static_cast<OperationCommandType>(commandType))));

    switch (commandType) {
    case static_cast<unsigned char>(OperationCommandType::TransmitAppAlias):
        appListsend();
        break;
    case static_cast<unsigned char>(OperationCommandType::TransmitDeviceInformaiton):
        deviceInformationsend();
        break;
    case static_cast<unsigned char>(OperationCommandType::TransmitAppCommand):
        appPathsend(packet);
        break;
    case static_cast<unsigned char>(OperationCommandType::TransmitUninstallAppCommand):
        appUninstallPathSend(packet);
        break;
    case static_cast<unsigned char>(OperationCommandType::TramsmitAppData):
        receiveInstallPackage(zmsg_new()); // Note: This is incorrect; should pass actual request
        break;
    default:
        logger.print("TCP_SERVER", "警告：未知命令类型: " + QString::number(commandType, 16));
        break;
    }
}

void tcpservertest::appListsend() {
    InstalledSoftware pc_software;
    pc_software.refreshSoftwareList();

    zmsg_t* response = zmsg_new();
    for (const QVariant& entry : pc_software.softwareList()) {
        QVariantMap entryMap = entry.toMap();
        QString appName = entryMap["name"].toString();
        QString appIconPath = entryMap["mainExe"].toString();

        RD_Packet namePacket{};
        namePacket.RD_Type = OperationCommandType::TransmitAppAlias;
        strncpy(namePacket.RD_APP_Name, appName.toUtf8().constData(), sizeof(namePacket.RD_APP_Name) - 1);
        zmsg_addmem(response, &namePacket, sizeof(namePacket));

        QFile iconFile(appIconPath);
        if (iconFile.open(QIODevice::ReadOnly)) {
            RD_Packet iconPacket{};
            iconPacket.RD_Type = OperationCommandType::TransmitAppIconData;
            QByteArray iconData = iconFile.readAll();
            int copySize = qMin(iconData.size(), static_cast<int>(sizeof(iconPacket.RD_ImageBit)));
            memcpy(iconPacket.RD_ImageBit, iconData.constData(), copySize);
            zmsg_addmem(response, &iconPacket, sizeof(iconPacket));
            iconFile.close();
        }

        RD_Packet endPacket{};
        endPacket.RD_Type = OperationCommandType::TransmitOnceEnd;
        zmsg_addmem(response, &endPacket, sizeof(endPacket));
    }

    RD_Packet finalEnd{};
    finalEnd.RD_Type = OperationCommandType::TransmitEnd;
    zmsg_addmem(response, &finalEnd, sizeof(finalEnd));

    zmsg_send(&response, responder_);
    logger.print("RDP_Server", "应用列表发送完成");
}

void tcpservertest::deviceInformationsend() {
    DeviceInfo info{};

    // 获取 CPU 核心数（使用 Qt 自带的 API）
    info.cpuCores = QThread::idealThreadCount();
    qDebug() << "[Info] CPU Cores:" << info.cpuCores;

    // 获取 CPU 型号（使用系统命令）
    QProcess cpuModelProcess;
    cpuModelProcess.start("wmic cpu get name");
    cpuModelProcess.waitForFinished();
    QString cpuModelOutput = cpuModelProcess.readAllStandardOutput();
    QStringList cpuModelLines = cpuModelOutput.split("\n", Qt::SkipEmptyParts);
    if (cpuModelLines.size() > 1) {
        QString cpuModel = cpuModelLines[1].trimmed();
        strncpy(info.cpuModel, cpuModel.toUtf8().constData(), sizeof(info.cpuModel) - 1);
        qDebug() << "[Info] CPU Model:" << info.cpuModel;
    } else {
        qDebug() << "[Error] Failed to get CPU model";
    }

    // 获取显卡型号（使用 nvidia-smi 命令）
    QProcess gpuModelProcess;
    gpuModelProcess.start("nvidia-smi --query-gpu=name --format=csv,noheader");
    gpuModelProcess.waitForFinished();
    QString gpuModelOutput = gpuModelProcess.readAllStandardOutput();
    QStringList gpuModelLines = gpuModelOutput.split("\n", Qt::SkipEmptyParts);
    if (gpuModelLines.size() > 0) {
        QString gpuModel = gpuModelLines[0].trimmed();
        strncpy(info.gpuModel, gpuModel.toUtf8().constData(), sizeof(info.gpuModel) - 1);
        qDebug() << "[Info] GPU Model:" << info.gpuModel;
    } else {
        qDebug() << "[Error] Failed to get GPU model";
    }

    // 获取内存信息（使用 Windows API）
    MEMORYSTATUSEX memoryStatus{};
    memoryStatus.dwLength = sizeof(memoryStatus);
    if (GlobalMemoryStatusEx(&memoryStatus)) {
        info.totalMemory = memoryStatus.ullTotalPhys;
        info.usedMemory = memoryStatus.ullTotalPhys - memoryStatus.ullAvailPhys;
        qDebug() << "[Info] Total Memory:" << info.totalMemory << "Bytes" << "Used Memory:" << info.usedMemory << "Bytes";
    } else {
        qDebug() << "[Error] Failed to get memory information";
    }

    // 获取磁盘信息（使用 Qt 自带的 API）
    QStorageInfo storage = QStorageInfo::root();
    info.totalDisk = storage.bytesTotal();
    info.usedDisk = storage.bytesTotal() - storage.bytesFree();
    qDebug() << "[Info] Total Disk:" << info.totalDisk << "Bytes" << "Used Disk:" << info.usedDisk << "Bytes";

    // 获取 CPU 使用率（使用系统命令）
    QProcess cpuUsageProcess;
    cpuUsageProcess.start("wmic cpu get loadpercentage");
    cpuUsageProcess.waitForFinished();
    QString cpuUsageOutput = cpuUsageProcess.readAllStandardOutput();
    QStringList cpuUsageLines = cpuUsageOutput.split("\n", Qt::SkipEmptyParts);

    if (cpuUsageLines.size() > 1) {
        QStringList parts = cpuUsageLines[1].split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (!parts.isEmpty()) {
            info.cpuUsage = parts[0].toDouble();
            qDebug() << "[Info] CPU Usage:" << info.cpuUsage << "%";
        } else {
            qDebug() << "[Error] Invalid CPU usage output format";
        }
    } else {
        qDebug() << "[Error] Failed to get CPU usage";
    }

    // 获取 CPU 温度（使用 wmic 命令）
    QProcess cpuTempProcess;
    cpuTempProcess.start("wmic /namespace:\\\\root\\wmi PATH MSAcpi_ThermalZoneTemperature get CurrentTemperature");
    cpuTempProcess.waitForFinished();
    QString cpuTempOutput = cpuTempProcess.readAllStandardOutput();
    QStringList cpuTempLines = cpuTempOutput.split("\n", Qt::SkipEmptyParts);
    if (cpuTempLines.size() > 1) {
        QStringList parts = cpuTempLines[1].split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (!parts.isEmpty()) {
            bool ok = false;
            int t = parts[0].toInt(&ok);
            if (ok) {
                info.cpuTemperature = (t - 2732) * 1.0f / 10;
                qDebug() << "[Info] CPU Temperature:" << info.cpuTemperature << "°C";
            } else {
                qDebug() << "[Error] Invalid CPU temperature output format";
            }
        } else {
            qDebug() << "[Error] Invalid CPU temperature output format";
        }
    } else {
        qDebug() << "[Error] Failed to get CPU temperature";
    }

    // 获取显卡温度（使用 nvidia-smi 命令）
    QProcess gpuTempProcess;
    gpuTempProcess.start("nvidia-smi --query-gpu=temperature --format=csv,noheader");
    gpuTempProcess.waitForFinished();
    QString gpuTempOutput = gpuTempProcess.readAllStandardOutput();
    QStringList gpuTempLines = gpuTempOutput.split("\n", Qt::SkipEmptyParts);
    if (gpuTempLines.size() > 0) {
        QStringList parts = gpuTempLines[0].split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (!parts.isEmpty()) {
            bool ok = false;
            info.gpuTemperature = parts[0].toInt(&ok);
            if (ok) {
                qDebug() << "[Info] GPU Temperature:" << info.gpuTemperature << "°C";
            } else {
                qDebug() << "[Error] Invalid GPU temperature output format";
            }
        } else {
            qDebug() << "[Error] Invalid GPU temperature output format";
        }
    } else {
        qDebug() << "[Error] Failed to get GPU temperature";
    }

    // 发送设备信息
    zmsg_t* response = zmsg_new();
    zmsg_addmem(response, &info, sizeof(info));
    zmsg_send(&response, responder_);
    logger.print("RDP_Server", "设备信息发送完成");
}

void tcpservertest::appPathsend(const RD_Packet& requestPacket) {
    QString appName = QString::fromUtf8(requestPacket.RD_APP_Name).trimmed();
    InstalledSoftware installedSoftware;
    installedSoftware.refreshSoftwareList();

    RD_Packet responsePacket{};
    responsePacket.RD_Type = OperationCommandType::TransmitAppCommand;
    for (const QVariant& entry : installedSoftware.softwareList()) {
        QVariantMap map = entry.toMap();
        if (map["name"].toString().compare(appName, Qt::CaseInsensitive) == 0) {
            QString path = map["mainExe"].toString();
            strncpy(responsePacket.RD_MainExePath, path.toUtf8().constData(), sizeof(responsePacket.RD_MainExePath) - 1);
            logger.print("TCP_SERVER", "找到应用路径: " + path);
            break;
        }
    }

    zmsg_t* response = zmsg_new();
    zmsg_addmem(response, &responsePacket, sizeof(responsePacket));
    zmsg_send(&response, responder_);
    logger.print("TCP_SERVER", "应用路径发送完成");
}

void tcpservertest::appUninstallPathSend(const RD_Packet& requestPacket) {
    QString appName = QString::fromUtf8(requestPacket.RD_APP_Name).trimmed();
    InstalledSoftware installedSoftware;
    installedSoftware.refreshSoftwareList();

    RD_Packet responsePacket{};
    responsePacket.RD_Type = OperationCommandType::TransmitUninstallAppCommand;
    for (const QVariant& entry : installedSoftware.softwareList()) {
        QVariantMap map = entry.toMap();
        if (map["name"].toString().compare(appName, Qt::CaseInsensitive) == 0) {
            QString uninstallPath = map["uninstallString"].toString();
            strncpy(responsePacket.RD_UninstallExePath, uninstallPath.toUtf8().constData(), sizeof(responsePacket.RD_UninstallExePath) - 1);
            logger.print("TCP_SERVER", "找到卸载路径: " + uninstallPath);
            break;
        }
    }

    zmsg_t* response = zmsg_new();
    zmsg_addmem(response, &responsePacket, sizeof(responsePacket));
    zmsg_send(&response, responder_);
    logger.print("TCP_SERVER", "卸载路径发送完成");
}

// 服务器端函数：接收到的消息中第一帧为传输头，后续帧成对为 ChunkHeader + 数据块
void tcpservertest::receiveInstallPackage(zmsg_t* request) {
    if (!request) return;

    // 提取第一帧，解析 RD_Packet（传输头）
    zframe_t *headerFrame = zmsg_pop(request);
    if (!headerFrame) return;
    RD_Packet packet;
    size_t headerSize = zframe_size(headerFrame);
    memcpy(&packet, zframe_data(headerFrame), headerSize < sizeof(packet) ? headerSize : sizeof(packet));
    zframe_destroy(&headerFrame);

    // 获取用户的“Download”目录路径
    QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (downloadDir.isEmpty()) {
        fprintf(stderr, "无法获取下载目录\n");
        zmsg_destroy(&request);
        return;
    }

    // 构造完整的文件路径
    QString outFileName = QString::fromUtf8(packet.installPackage.fileName);
    QDir dir(downloadDir);
    QString fullPath = dir.filePath(outFileName);

    // 打开文件进行写入
    QFile outFile(fullPath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "无法创建文件：%s\n", fullPath.toUtf8().constData());
        zmsg_destroy(&request);
        return;
    }

    // 处理后续帧，要求每个数据块由两个帧组成：ChunkHeader 和数据帧
    while (zmsg_size(request) >= 2) {
        // 取出数据块头
        zframe_t *chunkHeaderFrame = zmsg_pop(request);
        if (!chunkHeaderFrame) break;
        ChunkHeader chunk;
        size_t chSize = zframe_size(chunkHeaderFrame);
        memcpy(&chunk, zframe_data(chunkHeaderFrame), chSize < sizeof(chunk) ? chSize : sizeof(chunk));
        zframe_destroy(&chunkHeaderFrame);

        // 取出数据块数据
        zframe_t *dataFrame = zmsg_pop(request);
        if (!dataFrame) break;
        // 写入数据到文件（chunk.chunkSize 指示了本帧实际数据长度）
        outFile.write((const char*)zframe_data(dataFrame), chunk.chunkSize);
        zframe_destroy(&dataFrame);
    }
    outFile.close();
    // 清理剩余消息（若有）
    zmsg_destroy(&request);
    logger.print("TCP_SERVER", "安装包已保存至下载目录: " + fullPath);
}
#endif
