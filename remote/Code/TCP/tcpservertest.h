#ifndef TCPSERVERTEST_H
#define TCPSERVERTEST_H

#ifdef WIN32
#include "../AppData/tcpconnection.h"
#include "../LogUntils/AppLog.h"
#include "../AppData/devicedate.h"
#include "../AppData/installedsoftware.h"
#include <QObject>
#include <czmq.h>  // 使用 CZMQ 头文件

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
    RD_Packet recvPacket_;
    zsock_t* responder_ = nullptr;
    bool m_running;
};

#endif // WIN32
#endif // TCPSERVERTEST_H
