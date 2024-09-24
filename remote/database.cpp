#include "database.h"

DataBase::DataBase(QObject *parent) : QObject(parent), db(QSqlDatabase::addDatabase("QMYSQL"))
{
    db.setHostName("103.150.10.184"); // MySQL 服务器的 IP 地址
    db.setPort(28450);
    db.setDatabaseName("Remote"); // 数据库名称
    db.setUserName("kirito"); // 用户名
    db.setPassword("20030801hzy"); // 密码

    if (!db.open())
    {
        QSqlError error = db.lastError();
        qDebug() << "远端数据库连接失败:" << error.text();
    }
    else
    {
        qDebug() << "远端数据库连接成功";
    }
}

DataBase::~DataBase()
{
    db.close();
}
