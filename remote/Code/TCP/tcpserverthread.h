#ifndef TCPSERVERTHREAD_H
#define TCPSERVERTHREAD_H
#ifdef WIN32
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
    tcpservertest *m_server;
};
#endif
#endif // TCPSERVERTHREAD_H
