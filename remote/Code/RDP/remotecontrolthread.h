#ifndef REMOTECONTROLTHREAD_H
#define REMOTECONTROLTHREAD_H

#ifdef LINUX
#include <QThread>
#include <QDebug>
#include <atomic>
#include <QAtomicInt>
#include "remotecontrol.h" // 确保包含 RemoteControl 的头文件
#include "../LogUntils/AppLog.h"

class RemoteControlThread : public QThread
{
    Q_OBJECT

public:
    explicit RemoteControlThread(QObject *parent = nullptr, RemoteControl *remoteControl = nullptr);
    ~RemoteControlThread() override;

    Q_INVOKABLE void startConnection(const QString &hostname, const QString &username, const QString &password);
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
    bool m_running;
    QAtomicInt m_stopped;  // 使用 QAtomicInt 替代 bool
};

#endif // REMOTECONTROLTHREAD_H
#endif
