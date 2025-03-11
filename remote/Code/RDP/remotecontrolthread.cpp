#ifdef LINUX
#include "remotecontrolthread.h"

RemoteControlThread::RemoteControlThread(QObject *parent, RemoteControl *remoteControl)
    : QThread(parent), m_remoteControl(remoteControl), m_running(false), m_stopped(false)
{
    // 构造函数初始化
}

RemoteControlThread::~RemoteControlThread()
{
    stopConnection();  // 确保线程停止并清理资源
}

void RemoteControlThread::startConnection(const QString &hostname, const QString &username, const QString &password)
{
    this->m_hostname = hostname;
    this->m_username = username;
    this->m_password = password;
    qDebug() << m_hostname << " " << m_username << " " << m_password;
    m_running = true;
    std::atomic<int> m_stopped{0};  // Include <atomic>  // 重置为 0，表示未停止
    this->start();       // 启动线程
}

void RemoteControlThread::stopConnection()
{
    std::atomic<int> m_stopped{0};  // Include <atomic> // 设置停止标志
    if (this->isRunning()) {
        this->wait();    // 等待线程完全退出
        m_remoteControl->disconnect();  // 断开连接
    }
}

void RemoteControlThread::run()
{
    if (!m_remoteControl->initialize()) {
        emit errorOccurred("Failed to initialize FreeRDP");
        return;  // 直接返回，不调用 stopConnection()
    }
    if (!m_remoteControl->connect(m_hostname, m_username, m_password)) {
        emit errorOccurred("Failed to connect to RDP server");
        return;  // 直接返回，不调用 stopConnection()
    }

    if (m_running) {
        emit connectionFinished();      // 发出完成信号
        m_remoteControl->runEventLoop();  // 运行事件循环，需确保其内部检查 m_stopped
    }
}

#endif
