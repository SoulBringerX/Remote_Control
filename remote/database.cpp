#include "database.h"

DataBase::DataBase(QObject *parent) : QObject(parent), db(QSqlDatabase::addDatabase("QMYSQL"))
{
    db.setHostName("27.25.158.240"); // MySQL 服务器的 IP 地址
    db.setPort(14631);
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


bool DataBase::isLogin(const QString &userName, const QString &passWord)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM userdata WHERE username = :username");
    query.bindValue(":username", userName);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError();
        return false;
    }

    // 检查是否有匹配的用户
    if (query.next()) {
        // 获取查询结果中的密码
        QString storedPassword = query.value(0).toString();

        // 比较提供的密码和存储的密码
        if (storedPassword == passWord)
        {
            return true;
        } else {
            return false;
        }
    } else {
        // 没有找到用户名
        return false;
    }
}

bool isRegister(const QString &userName, const QString &passWord);

