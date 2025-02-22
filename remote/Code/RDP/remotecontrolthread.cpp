#ifdef LINUX
#include "remotecontrolthread.h"

RemoteControlThread::RemoteControlThread(QObject *parent, RemoteControl *remoteControl)
    : QThread(parent), m_remoteControl(remoteControl), m_running(false), m_stopped(false)
{
    // 构造函数初始化
}

void RemoteControlThread::startConnection(const QString &hostname, const QString &username, const QString &password)
{
    this->m_hostname = hostname;
    this->m_username = username;
    this->m_password = password;
    m_running = true;
    m_stopped = false;
    this->start(); // 启动线程
}

void RemoteControlThread::stopConnection()
{
    m_stopped = true;
    if (this->isRunning()) {
        this->wait(); // 等待线程结束
    }
}

void RemoteControlThread::run()
{
    if (!m_remoteControl->initialize()) {
        emit errorOccurred("Failed to initialize FreeRDP");
        return;
    }

    if (!m_remoteControl->connect(m_hostname, m_username, m_password)) {
        emit errorOccurred("Failed to connect to RDP server");
        return;
    }

    if (m_running) {
        m_remoteControl->runEventLoop();
    }

    m_remoteControl->disconnect();
    emit connectionFinished();
}
#endif
