#ifdef LINUX
#include "remotecontrolthread.h"
#include <QMetaObject>

RemoteControlThread::RemoteControlThread(QObject *parent, RemoteControl *remoteControl)
    : QThread(parent),
      m_remoteControl(remoteControl),
      m_exitRequested(0)
{
    // 构造函数初始化
}

RemoteControlThread::~RemoteControlThread()
{
    stopConnection();  // 确保线程停止并清理资源
}

void RemoteControlThread::startConnection(const QString &hostname, const QString &username, const QString &password)
{
    m_hostname = hostname;
    m_username = username;
    m_password = password;
    qDebug() << m_hostname << " " << m_username << " " << m_password;
    m_exitRequested = 0;  // 重置为 0，表示未退出
    this->start();  // 启动线程
}

void RemoteControlThread::stopConnection()
{
    // 请求退出
    m_exitRequested = 1;
    if (isRunning() && m_remoteControl) {
        // 异步调用断开连接（在 RemoteControl 所在的线程中执行）
        QMetaObject::invokeMethod(m_remoteControl, "onDisconnectRequested", Qt::QueuedConnection);
        // 等待线程结束
        wait();
    }
}

void RemoteControlThread::run()
{
    // 如果在开始前就请求退出，则直接返回
    if (m_exitRequested == 1)
        return;

    // 初始化 FreeRDP 环境
    if (!m_remoteControl->initialize()) {
        emit errorOccurred("Failed to initialize FreeRDP");
        return;
    }
    // 建立连接
    if (!m_remoteControl->connect(m_hostname, m_username, m_password)) {
        emit errorOccurred("Failed to connect to RDP server");
        return;
    }

    emit connectionFinished();

    // 进入事件循环，要求 runEventLoop() 内部能够检测断开操作并退出
    m_remoteControl->runEventLoop();
}
#endif
