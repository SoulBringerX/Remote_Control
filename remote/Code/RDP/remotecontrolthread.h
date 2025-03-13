#ifndef REMOTECONTROLTHREAD_H
#define REMOTECONTROLTHREAD_H

#ifdef LINUX
#include <QThread>
#include <QDebug>
#include <QAtomicInt>
#include "remotecontrol.h" // 包含 RemoteControl 的头文件
#include "../LogUntils/AppLog.h"

class RemoteControlThread : public QThread
{
    Q_OBJECT

public:
    explicit RemoteControlThread(QObject *parent = nullptr, RemoteControl *remoteControl = nullptr);
    ~RemoteControlThread() override;

    // 启动连接（只会尝试一次连接和启动事件循环）
    Q_INVOKABLE void startConnection(const QString &hostname, const QString &username, const QString &password);
    // 请求断开连接并退出线程
    Q_INVOKABLE void stopConnection();

signals:
    void errorOccurred(const QString &error);
    void connectionFinished();

protected:
    void run() override;

private:
    RemoteControl *m_remoteControl;
    QString m_hostname;
    QString m_username;
    QString m_password;
    QAtomicInt m_exitRequested;  // 0: 正常运行，1: 请求退出
};

#endif // REMOTECONTROLTHREAD_H
#endif
