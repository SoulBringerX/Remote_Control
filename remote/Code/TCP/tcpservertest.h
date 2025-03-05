#ifndef TCPSERVERTEST_H
#define TCPSERVERTEST_H
#ifdef WIN32
#include "../AppData/tcpconnection.h"
#include "../LogUntils/AppLog.h"
#include "../AppData/devicedate.h"
#include "../AppData/installedsoftware.h"
#include <QThread>
class tcpservertest: public QObject
{
public:
    // 类的初始化函数用于初始化服务器的相关配置
    tcpservertest();
    // 传输应用名称
    void appNamesend();
private:
    RD_Packet recvPacket_;
    void* context_ = nullptr;
    void* responder_ = nullptr;
};

#endif // TCPSERVERTEST_H
#endif
