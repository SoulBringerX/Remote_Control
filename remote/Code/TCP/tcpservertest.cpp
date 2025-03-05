// tcpservertest.cpp
#include "tcpservertest.h"
#include "../AppData/installedsoftware.h"
#include <QDebug>

tcpservertest::tcpservertest() {
    context_ = zmq_ctx_new();
    responder_ = zmq_socket(context_, ZMQ_REP);
    int rc = zmq_bind(responder_, "tcp://*:5555");
    logger.print("RDP_Server", "StartListening >>>>>>");

    assert(rc == 0);
}

void tcpservertest::exec() {
    while (1) {
        zmq_recv(responder_, &recvPacket_, sizeof(recvPacket_), 0);
        logger.print("RDP_Server", "recvPacket数据包大小：" + QString::number(sizeof(recvPacket_)));

        if (recvPacket_.RD_Type == OperationCommandType::TransmitAppAlias) {
            appListsend();
        }

        zmq_send(responder_, "World", 5, 0);
    }
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
