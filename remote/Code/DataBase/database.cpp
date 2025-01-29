#include "database.h"

DataBase* DataBase::instance = nullptr;
QMutex DataBase::mutex; // 静态成员变量初始化

DataBase::DataBase(QObject *parent) : QObject(parent), db(QSqlDatabase::addDatabase("QMYSQL","unique_connection_name"))
{
    db.setHostName("113.45.183.22"); // MySQL 服务器的 IP 地址
    db.setPort(3306);
    db.setDatabaseName("Remote"); // 数据库名称
    db.setUserName("kirito"); // 用户名
    db.setPassword("123456"); // 密码

    if (!db.open())
    {
        QSqlError error = db.lastError();
        logger.print("远端数据库连接失败:", error.text());
    }
    else
    {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString dateTimeString = currentDateTime.toString("yyyy-MM-dd HH:mm:ss");
        logger.print("数据库连接成功", dateTimeString);
    }
}

DataBase::~DataBase()
{
    db.close();
    QSqlDatabase::removeDatabase("unique_connection_name");
}

DataBase* DataBase::getInstance()
{
    QMutexLocker locker(&mutex); // 使用静态成员变量
    if (instance == nullptr) {
        instance = new DataBase();
    }
    return instance;
}

bool DataBase::isLogin(const QString &userName, const QString &passWord)
{
    QSqlQuery query(db);
    query.prepare("SELECT password FROM users WHERE account = :username");
    query.bindValue(":username", userName);

    if (!query.exec()) {
        logger.print("查询执行失败", query.lastError().text());
        return false;
    }

    if (query.next()) {
        return query.value(0).toString() == passWord;
    }

    return false;
}

bool DataBase::isRegister(const QString &userName, const QString &passWord)
{
    // 检查用户名是否已存在
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE account = :username");
    query.bindValue(":username", userName);

    if (!query.exec()) {
        logger.print("查询执行失败", query.lastError().text());
        return false;
    }

    int count = 0;
    if (query.next()) {
        count = query.value(0).toInt();
    }

    if (count > 0) {
        // 用户名已存在，注册失败
        logger.print("数据库已存在该账户数据", query.lastError().text());
        return false;
    }
    return true;
}

bool DataBase::userRegister(const QString &userName, const QString &passWord){
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (account, password) VALUES (:username, :password)");
    query.bindValue(":username", userName);
    query.bindValue(":password", passWord);

    if (!query.exec()) {
        logger.print("注册失败", query.lastError().text());
        return false;
    }

    return true;
}
