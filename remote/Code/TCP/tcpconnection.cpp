#include "tcpconnection.h"
#include <iostream>
#include <stdexcept>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QByteArray>

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

    // 将应用列表写入到配置文件
    this->writeAppListToConfig(appList, TCP_IP);

    emit appListReceived(appList);
    return appList;
}

QVariantMap tcpConnection::receiveDeviceInfo() {
    QVariantMap deviceInfo;

    if (!sockfd_) {
        emit connectionError("无效的 socket，无法请求设备信息");
        return deviceInfo;
    }

    // 发送设备信息请求
    RD_Packet requestPacket;
    memset(&requestPacket, 0, sizeof(requestPacket));
    requestPacket.RD_Type = OperationCommandType::TransmitDeviceInformaiton;
    if (zsock_send(sockfd_, "b", &requestPacket, sizeof(requestPacket)) != 0) {
        emit connectionError("请求设备信息失败");
        return deviceInfo;
    }
    qDebug() << "📡 已发送设备信息请求";

    // **接收 ZeroMQ 消息**
    zmsg_t* reply = zmsg_recv(sockfd_);
    if (!reply) {
        emit connectionError("接收设备信息失败");
        return deviceInfo;
    }

    // 读取第一个数据帧
    zframe_t* frame = zmsg_pop(reply);
    if (frame && zframe_size(frame) == sizeof(DeviceInfo)) {
        DeviceInfo info;
        memcpy(&info, zframe_data(frame), sizeof(DeviceInfo));
        zframe_destroy(&frame);

        // **存储设备信息**
        deviceInfo["cpuModel"] = QString::fromUtf8(info.cpuModel).trimmed();
        deviceInfo["cpuCores"] = info.cpuCores;
        deviceInfo["cpuUsage"] = info.cpuUsage;
        deviceInfo["cpuTemperature"] = info.cpuTemperature;
        deviceInfo["totalMemory"] = info.totalMemory;
        deviceInfo["usedMemory"] = info.usedMemory;
        deviceInfo["totalDisk"] = info.totalDisk;
        deviceInfo["usedDisk"] = info.usedDisk;
        deviceInfo["gpuModel"] = QString::fromUtf8(info.gpuModel).trimmed();
        deviceInfo["gpuTemperature"] = info.gpuTemperature;

        // **打印设备信息**
        qDebug() << "✅ 设备信息解析成功:";
        qDebug() << "   🖥️ CPU 型号: " << deviceInfo["cpuModel"].toString();
        qDebug() << "   🧩 CPU 核心数: " << deviceInfo["cpuCores"].toInt();
        qDebug() << "   ⚡ CPU 占用率: " << deviceInfo["cpuUsage"].toDouble() << "%";
        qDebug() << "   🌡️ CPU 温度: " << deviceInfo["cpuTemperature"].toDouble() << "°C";
        qDebug() << "   💾 总内存: " << deviceInfo["totalMemory"].toULongLong() << " Bytes";
        qDebug() << "   📊 已用内存: " << deviceInfo["usedMemory"].toULongLong() << " Bytes";
        qDebug() << "   🗄️ 总磁盘: " << deviceInfo["totalDisk"].toULongLong() << " Bytes";
        qDebug() << "   📂 已用磁盘: " << deviceInfo["usedDisk"].toULongLong() << " Bytes";
        qDebug() << "   🎮 GPU 型号: " << deviceInfo["gpuModel"].toString();
        qDebug() << "   🌡️ GPU 温度: " << deviceInfo["gpuTemperature"].toDouble() << "°C";
    }

    zmsg_destroy(&reply);
    emit deviceInfoReceived(deviceInfo);
    DeviceInfoManager::getInstance()->updateDeviceInfo(deviceInfo);
    return deviceInfo;
}

// 获取远端App的EXE执行路径
QString tcpConnection::receiveAppPath(const QString& AppName){
    if (!sockfd_) {
        emit connectionError("无效的 socket，无法请求应用路径");
        return QString();
    }

    // 构造请求数据包，包含要查询的应用名称
    RD_Packet requestPacket;
    memset(&requestPacket, 0, sizeof(requestPacket));
    requestPacket.RD_Type = OperationCommandType::TransmitAppCommand;
    strncpy(requestPacket.RD_APP_Name, AppName.toUtf8().constData(), sizeof(requestPacket.RD_APP_Name) - 1);

    // 发送请求数据包
    if (zsock_send(sockfd_, "b", &requestPacket, sizeof(requestPacket)) != 0) {
        emit connectionError("请求应用路径失败：" + AppName);
        return QString();
    }
    qDebug() << "📡 已发送应用路径请求：" << AppName;

    // **接收 ZeroMQ 消息**
    zmsg_t* reply = zmsg_recv(sockfd_);
    if (!reply) {
        emit connectionError("接收应用路径失败：" + AppName);
        return QString();
    }

    QString appPath;
    zframe_t* frame = zmsg_pop(reply);
    if (frame && zframe_size(frame) == sizeof(RD_Packet)) {
        RD_Packet packet;
        memcpy(&packet, zframe_data(frame), sizeof(RD_Packet));
        zframe_destroy(&frame);

        // **检查是否是有效的路径返回**
        if (packet.RD_Type == OperationCommandType::TransmitAppCommand) {
            appPath = QString::fromUtf8(packet.RD_MainExePath).trimmed();
            qDebug() << "✅ 远程应用路径：" << appPath;
        } else {
            qDebug() << "⚠️ 收到无效的应用路径数据";
        }
    }

    zmsg_destroy(&reply);
    return appPath;
}

// 获取远端App的卸载程序执行路径
QString tcpConnection::receiveUninstallAppPath(const QString& AppName) {
    if (!sockfd_) {
        // Use emit if tcpConnection inherits QObject and declares the signal
        // emit connectionError("无效的 socket，无法请求卸载路径");
        qWarning() << "无效的 socket，无法请求卸载路径"; // Use qWarning if not using signals here
        return QString();
    }

    // 构造请求数据包，包含要查询的应用名称
    RD_Packet requestPacket;
    memset(&requestPacket, 0, sizeof(requestPacket));
    // *** 设置请求类型为卸载路径请求 ***
    requestPacket.RD_Type = OperationCommandType::TransmitUninstallAppCommand;
    // 仍然需要应用名称来识别是哪个应用的卸载程序
    strncpy(requestPacket.RD_APP_Name, AppName.toUtf8().constData(), sizeof(requestPacket.RD_APP_Name) - 1);

    // 发送请求数据包
    if (zsock_send(sockfd_, "b", &requestPacket, sizeof(requestPacket)) != 0) {
        QString errorMsg = "请求卸载路径失败：" + AppName;
        qWarning() << errorMsg; // Use qWarning if not using signals here
        return QString();
    }
    qDebug() << "📡 已发送卸载路径请求：" << AppName;

    // **接收 ZeroMQ 消息**
    zmsg_t* reply = zmsg_recv(sockfd_);
    if (!reply) {
        QString errorMsg = "接收卸载路径失败：" + AppName;
        qWarning() << errorMsg; // Use qWarning if not using signals here
        return QString();
    }

    QString uninstallPath;
    zframe_t* frame = zmsg_pop(reply);
    if (frame) { // Check if frame is not null before using it
        if (zframe_size(frame) == sizeof(RD_Packet)) {
            RD_Packet packet;
            memcpy(&packet, zframe_data(frame), sizeof(RD_Packet));
            if (packet.RD_Type == OperationCommandType::TransmitUninstallAppCommand) {
                 // *** 从对应的卸载路径字段提取数据 (假设为 RD_UninstallPath) ***
                // IMPORTANT: Ensure RD_Packet struct actually HAS a RD_UninstallPath field!
                uninstallPath = QString::fromUtf8(packet.RD_UninstallExePath).trimmed();
                if (!uninstallPath.isEmpty()) {
                    qDebug() << "✅ 远程卸载路径：" << uninstallPath;
                } else {
                     qDebug() << "⚠️ 收到空的卸载路径数据 for" << AppName;
                }
            } else {
                qDebug() << "⚠️ 收到无效的卸载路径数据 (Type mismatch: expected"
                         << static_cast<unsigned char>(OperationCommandType::TransmitUninstallAppCommand)
                         << ", got" << static_cast<unsigned char>(packet.RD_Type) << ") for" << AppName;
            }
        } else {
             qDebug() << "⚠️ 收到大小错误的卸载路径响应帧 (Expected" << sizeof(RD_Packet) << ", Got" << zframe_size(frame) << ") for" << AppName;
        }
         zframe_destroy(&frame); // Destroy the frame after processing
    } else {
         qDebug() << "⚠️ 收到空的响应消息 for" << AppName;
    }


    zmsg_destroy(&reply); // Destroy the message container
    return uninstallPath;
}

bool tcpConnection::sendInstallPackage(const QString &filePath) {
    // 打开安装包文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件:" << filePath;
        return false;
    }

    QFileInfo fileInfo(file);
    qint64 fileSize = fileInfo.size();
    QString fileName = fileInfo.fileName();

    // 构造传输头 RD_Packet_Net，记录文件元数据
    RD_Packet packet;
    packet.RD_Type = OperationCommandType::TramsmitAppData;  // 指定数据传输类型为安装包数据
    // 根据实际需求，可填充 RD_IP、RD_Username、RD_Password 等字段
    QByteArray filePathBA = filePath.toUtf8();
    QByteArray fileNameBA = fileName.toUtf8();
    strncpy(packet.installPackage.filePath, filePathBA.constData(), sizeof(packet.installPackage.filePath) - 1);
    strncpy(packet.installPackage.fileName, fileNameBA.constData(), sizeof(packet.installPackage.fileName) - 1);
    packet.installPackage.fileSize = fileSize;

    // 创建 czmq 多帧消息
    zmsg_t *msg = zmsg_new();
    if (!msg) {
        qWarning() << "无法创建 czmq 消息";
        file.close();
        return false;
    }

    // 将 RD_Packet_Net 数据打包为第一帧
    zframe_t *headerFrame = zframe_new(&packet, sizeof(packet));
    if (!headerFrame) {
        qWarning() << "无法创建传输头帧";
        zmsg_destroy(&msg);
        file.close();
        return false;
    }
    zmsg_append(msg, &headerFrame);

    // 按固定块大小读取文件内容，并分块传输
    const int CHUNK_SIZE = 1024;  // 每块传输 1024 字节，根据需要调整
    while (!file.atEnd()) {
        QByteArray chunkData = file.read(CHUNK_SIZE);
        if (chunkData.isEmpty()) {
            break;
        }

        // 构造数据块头
        ChunkHeader chunk;
        memset(&chunk, 0, sizeof(chunk));
        chunk.RD_Type = OperationCommandType::TramsmitAppData;  // 与传输头中类型一致
        strncpy(chunk.fileName, fileNameBA.constData(), sizeof(chunk.fileName) - 1);
        chunk.chunkSize = chunkData.size();

        // 将数据块头添加为一帧
        zframe_t *chunkHeaderFrame = zframe_new(&chunk, sizeof(chunk));
        if (!chunkHeaderFrame) {
            qWarning() << "无法创建数据块头帧";
            zmsg_destroy(&msg);
            file.close();
            return false;
        }
        zmsg_append(msg, &chunkHeaderFrame);

        // 将数据块内容作为下一帧添加
        zframe_t *dataFrame = zframe_new(chunkData.constData(), chunkData.size());
        if (!dataFrame) {
            qWarning() << "无法创建数据块数据帧";
            zmsg_destroy(&msg);
            file.close();
            return false;
        }
        zmsg_append(msg, &dataFrame);
    }
    file.close();

    // 发送整个多帧消息，假设 tcpConnection 内已有 czmq 套接字成员 socket_
    int rc = zmsg_send(&msg, sockfd_);
    if (rc != 0) {
        qWarning() << "发送安装包失败";
        return false;
    }
    return true;
}

// 将应用列表写入到配置文件
void tcpConnection::writeAppListToConfig(const QVariantList &appList, const QString &ip) {
    // 创建或打开配置文件
    QDir configDir(QDir::homePath());
    if (!configDir.exists("config")) {
        configDir.mkdir("config");
    }
    configDir.cd("config");

    // 使用目标IP作为配置文件名
    QString configFileName = configDir.filePath(ip + "_app_list.ini");
    QSettings settings(configFileName, QSettings::IniFormat);

    // 清空现有配置
    settings.clear();

    // 写入应用列表
    for (int i = 0; i < appList.size(); ++i) {
        const QVariantMap &appInfo = appList[i].toMap();
        settings.beginGroup(QString("App_%1").arg(i));

        // 写入应用信息
        settings.setValue("name", appInfo["name"].toString());
        settings.setValue("mainExe", appInfo["mainExe"].toString());
        settings.setValue("uninstallExe", appInfo["uninstallExe"].toString());

        // 图标数据需要进行 Base64 编码，以便存储为字符串
        QByteArray iconData = appInfo["iconData"].toByteArray();
        settings.setValue("iconData", iconData.toBase64());

        settings.endGroup();
    }

    // 确保所有更改都已写入到文件中
    settings.sync();

    if (settings.status() != QSettings::NoError) {
        qDebug() << "[Error] Failed to write application list to config file:" << settings.status();
    } else {
        qDebug() << "[Info] Application list written to config file successfully:" << configFileName;
    }
}

// 线程管理类实现
TcpThread::TcpThread(QObject *parent) : QThread(parent), tcpConn(nullptr) {}

TcpThread::~TcpThread() {
    if (tcpConn) {
        tcpConn->deleteLater();
    }
}

void TcpThread::run() {
    tcpConn = new tcpConnection();
    tcpConn->moveToThread(this);  // Move to the worker thread
    emit tcpReady(tcpConn);  // Emit signal when tcpConnection is ready
    exec();  // Start the event loop for the thread
}
