#ifndef REMOTECONTROLTHREAD_H
#define REMOTECONTROLTHREAD_H

#ifdef LINUX
#include <QThread>
#include <QDebug>
#include "remotecontrol.h" // 确保包含 RemoteControl 的头文件
#include "../LogUntils/AppLog.h"

class RemoteControlThread : public QThread
{
    Q_OBJECT // 必须包含以支持信号和槽
public:
    explicit RemoteControlThread(QObject *parent = nullptr, RemoteControl *remoteControl = nullptr);

    Q_INVOKABLE void startConnection(const QString &hostname, const QString &username, const QString &password);
    Q_INVOKABLE void stopConnection();

signals:
    void errorOccurred(const QString& message);
    void connectionFinished();

protected:
    void run() override;

private:
    RemoteControl* m_remoteControl;
    QString m_hostname, m_username, m_password;
    bool m_running, m_stopped;
};

#endif // REMOTECONTROLTHREAD_H
#endif
