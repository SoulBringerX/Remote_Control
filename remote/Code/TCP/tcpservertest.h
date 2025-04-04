#ifndef TCPSERVERTEST_H
#define TCPSERVERTEST_H

#ifdef WIN32
#include "../AppData/tcpconnection.h"
#include "../LogUntils/AppLog.h"
#include "../AppData/devicedate.h"
#include "../AppData/installedsoftware.h"
#include <QObject>
#include <string.h>
#include <QSysInfo>
#include <QStorageInfo>
#include <windows.h>
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>
#include <czmq.h>

class tcpservertest : public QObject
{
    Q_OBJECT
public:
    tcpservertest();
    ~tcpservertest();
    void exec();
    void stop();

private:
    void appListsend();
    void deviceInformationsend();
    void appPathsend(const RD_Packet& requestPacket);
    void handlePacket(const RD_Packet& packet);
    void appUninstallPathSend(const RD_Packet& requestPacket);
    void receiveInstallPackage(zmsg_t* request);

    RD_Packet recvPacket_;              // 接收到的数据包
    zsock_t* responder_ = nullptr;      // ZMQ REP 套接字
    bool m_running = false;             // 服务器运行状态
    QFile currentFile_;                 // 当前接收的文件
    qint64 expectedFileSize_ = 0;       // 期望的文件大小
    qint64 receivedBytes_ = 0;          // 已接收的字节数
    QString currentFileName_;           // 当前文件名
    QString receiveDir_ = "C:/ServerReceivedFiles/";  // 预定义接收目录
};

#endif // WIN32
#endif // TCPSERVERTEST_H
