#include "tcpserverthread.h"
#include "tcpservertest.h"
#include <QDebug>

TcpServerThread::TcpServerThread(QObject *parent)
    : QThread(parent), m_running(true)
{
}

void TcpServerThread::run()
{
    tcpservertest server;
    while (m_running) {
        server.exec();
        qDebug() << "Server thread running";
    }
    qDebug() << "Server thread stopped";
    emit finished();
}

void TcpServerThread::stop()
{
    m_running = false;
    qDebug() << "Server thread stop requested";
}
