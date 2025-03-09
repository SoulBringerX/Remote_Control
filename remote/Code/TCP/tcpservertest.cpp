#ifdef WIN32
// tcpservertest.cpp
#include "tcpservertest.h"
#include "../AppData/installedsoftware.h"
#include <QDebug>
#include <chrono>
#include <QElapsedTimer>  // Qt 提供的计时器

// 构造函数：初始化 ZMQ 并设置运行标志
tcpservertest::tcpservertest() : m_running(true) {  // [!++ 初始化 m_running +!]
    context_ = zsock_new(ZMQ_REQ);
    assert(context_);
    responder_ = zmq_socket(context_, ZMQ_REP);
    int rc = zmq_bind(responder_, "tcp://*:5555");
    logger.print("RDP_Server", "StartListening >>>>>>");
    assert(rc == 0);
}

// 核心执行逻辑：改用非阻塞接收
void tcpservertest::exec() {
    QElapsedTimer timer;
    timer.start();
    int lastStatusTime = 0;  // 记录上一次状态输出的秒数

    while (m_running) {
        zmq_pollitem_t items[] = { { responder_, 0, ZMQ_POLLIN, 0 } };
        int poll_rc = zmq_poll(items, 1, 500);  // 500ms 超时

        if (poll_rc == -1) {
            logger.print("RDP_Server", "ZMQ poll error");
            break;
        }

        // 计算已等待的秒数（QElapsedTimer 返回毫秒数）
        int elapsedSec = timer.elapsed() / 1000;

        // 每隔 5 秒输出一次当前等待状态
        if (elapsedSec - lastStatusTime >= 5) {
            logger.print("RDP_Server", QString("当前等待客户端连接，已等待 %1 秒").arg(elapsedSec));
            lastStatusTime = elapsedSec;
        }

        // 超过 30 秒则退出循环并提示用户
        if (elapsedSec >= 30) {
            logger.print("RDP_Server", "等待客户端连接超时30秒，自动关闭线程");
            break;
        }

        // 如果有数据到达则处理
        if (items[0].revents & ZMQ_POLLIN) {
            zmq_recv(responder_, &recvPacket_, sizeof(recvPacket_), 0);
            logger.print("RDP_Server", "recvPacket数据包大小：" + QString::number(sizeof(recvPacket_)));

            if (recvPacket_.RD_Type == OperationCommandType::TransmitAppAlias) {
                appListsend();
            }
            zmq_send(responder_, "WAIT", 4, 0);
        }
    }
}


// [!++ 新增停止方法：触发循环退出 +!]
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

        // 发送应用名称
        RD_Packet namePacket;
        namePacket.RD_Type = OperationCommandType::TransmitAppAlias;
        strncpy(namePacket.RD_APP_Name, appName.toStdString().c_str(), sizeof(namePacket.RD_APP_Name) - 1);
        zmq_send(responder_, &namePacket, sizeof(namePacket), 0);

        // 发送应用图标数据
        RD_Packet iconPacket;
        iconPacket.RD_Type = OperationCommandType::TransmitAppIconData;
        // 这里假设图标数据是字节数组，需要从文件中读取
        QFile iconFile(appIconPath);
        if (iconFile.open(QIODevice::ReadOnly)) {
            QByteArray iconData = iconFile.readAll();
            int dataSize = qMin(iconData.size(), static_cast<int>(sizeof(iconPacket.RD_ImageBit)));
            memcpy(iconPacket.RD_ImageBit, iconData.constData(), dataSize);
            zmq_send(responder_, &iconPacket, sizeof(iconPacket), 0);
        }

        // 发送结束标志
        RD_Packet endPacket;
        endPacket.RD_Type = OperationCommandType::TransmitEnd;
        zmq_send(responder_, &endPacket, sizeof(endPacket), 0);
    }
}

// tcpservertest.cpp
tcpservertest::~tcpservertest() {
    zmq_close(responder_);
    zmq_ctx_destroy(context_);
}
#endif
