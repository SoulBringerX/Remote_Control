#ifndef TCPSERVERTHREAD_H
#define TCPSERVERTHREAD_H

#include <QThread>
#include "tcpservertest.h"

class TcpServerThread : public QThread
{
    Q_OBJECT

public:
    explicit TcpServerThread(QObject *parent = nullptr);
    void stop();

protected:
    void run() override;

signals:
    void finished();

private:
    bool m_running;
};

#endif // TCPSERVERTHREAD_H
