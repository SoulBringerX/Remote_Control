#ifndef TCPSERVERTEST_H
#define TCPSERVERTEST_H

#ifdef WIN32
#include "../AppData/tcpconnection.h"
#include "../LogUntils/AppLog.h"
#include "../AppData/devicedate.h"
#include "../AppData/installedsoftware.h"
#include <QObject>
#include <zmq.h>

class tcpservertest : public QObject
{
    Q_OBJECT
public:
    tcpservertest();
    ~tcpservertest();
    void exec();
    void stop();  // [!++ 新增停止方法 +!]

private:
    void appListsend();
    RD_Packet recvPacket_;
    void* context_ = nullptr;
    void* responder_ = nullptr;
    bool m_running;  // [!++ 控制循环的标记 +!]
};

#endif // WIN32
#endif // TCPSERVERTEST_H
