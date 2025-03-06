#ifdef WIn_32
#include "tcpserverthread.h"
#include "tcpservertest.h"
#include <QDebug>

// tcpserverthread.cpp
TcpServerThread::TcpServerThread(QObject *parent)
    : QThread(parent), m_running(false), m_server(nullptr) {}

// tcpserverthread.cpp
void TcpServerThread::run() {
    m_running = true;
    m_server = new tcpservertest();  // [!++ 动态创建对象 +!]
    m_server->exec();               // 进入主循环
    delete m_server;                // 安全释放
    m_server = nullptr;
    emit finished();
}

void TcpServerThread::stop() {
    m_running = false;
    if (m_server) {
        m_server->stop();  // [!++ 联动停止服务器 +!]
    }
}
#endif
