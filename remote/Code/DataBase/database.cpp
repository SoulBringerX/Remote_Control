#include "database.h"
#include <QTimer>

DataBase* DataBase::instance = nullptr;
QMutex DataBase::mutex; // 静态成员变量初始化

// 构造函数
DataBase::DataBase(QObject *parent) : QObject(parent),
    db(QSqlDatabase::addDatabase("QMYSQL", "unique_connection_name"))
{
    // 硬编码数据库连接参数
    db.setHostName("113.45.183.22"); // MySQL 服务器的 IP 地址
    db.setPort(3306);
    db.setDatabaseName("Remote");    // 数据库名称
    db.setUserName("kirito");        // 用户名
    db.setPassword("123456");        // 密码

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

    // 初始化心跳定时器
    keepAliveTimer = new QTimer(this);
    connect(keepAliveTimer, &QTimer::timeout, this, &DataBase::sendHeartbeat);
    keepAliveTimer->start(300000); // 每5分钟发送一次心跳
}

// 析构函数
DataBase::~DataBase()
{
    keepAliveTimer->stop();
    delete keepAliveTimer;
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase("unique_connection_name");
}

// 单例模式获取实例
DataBase* DataBase::getInstance()
{
    QMutexLocker locker(&mutex);
    if (instance == nullptr) {
        instance = new DataBase();
    }
    return instance;
}

// 检查并重连数据库
bool DataBase::checkAndReconnect()
{
    if (db.isOpen()) {
        // 执行简单查询验证连接是否有效
        QSqlQuery testQuery(db);
        if (testQuery.exec("SELECT 1")) {
            return true;
        }
    }

    // 关闭并移除旧连接
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase("unique_connection_name");

    // 重新创建连接
    db = QSqlDatabase::addDatabase("QMYSQL", "unique_connection_name");
    db.setHostName("113.45.183.22");
    db.setPort(3306);
    db.setDatabaseName("Remote");
    db.setUserName("kirito");
    db.setPassword("123456");

    if (!db.open()) {
        QSqlError error = db.lastError();
        logger.print("数据库重连失败:", error.text());
        return false;
    }
    logger.print("数据库重连成功", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    return true;
}

// 发送心跳保持连接活跃
void DataBase::sendHeartbeat()
{
    if (checkAndReconnect()) {
        QSqlQuery query(db);
        query.exec("SELECT 1");
    }
}

// 用户登录
bool DataBase::isLogin(const QString &userName, const QString &passWord)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    Account::Remote_username = userName;
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

// 检查是否可以注册
bool DataBase::isRegister(const QString &userName, const QString &passWord)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE account = :username");
    query.bindValue(":username", userName);

    if (!query.exec()) {
        logger.print("查询执行失败", query.lastError().text());
        return false;
    }

    if (query.next()) {
        int count = query.value(0).toInt();
        if (count > 0) {
            logger.print("数据库已存在该账户数据", "");
            return false;
        }
    }
    return true;
}

// 用户注册
bool DataBase::userRegister(const QString &userName, const QString &passWord)
{
    if (!checkAndReconnect()) {
        return false;
    }
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

// 推送设备数据
bool DataBase::pushDeviceData(const QString& hostname, const QString& username, const QString& password)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);

    query.prepare("SELECT user_id FROM users WHERE account = :account");
    query.bindValue(":account", Account::Remote_username);
    if (!query.exec()) {
        qDebug() << "[Error] Failed to get user_id:" << query.lastError().text();
        return false;
    }

    int userId = -1;
    if (query.next()) {
        userId = query.value("user_id").toInt();
        qDebug() << "[Info] Found user_id =" << userId;
    } else {
        qDebug() << "[Error] No user found for account:" << Account::Remote_username;
        return false;
    }

    query.prepare(
        "INSERT INTO devices (user_id, device_name, ip_address, password, last_connected, is_trusted) "
        "VALUES (:user_id, :device_name, :ip_address, :password, :last_connected, :is_trusted)"
    );
    query.bindValue(":user_id", userId);
    query.bindValue(":device_name", username);
    query.bindValue(":ip_address", hostname);
    query.bindValue(":password", password);
    query.bindValue(":last_connected", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":is_trusted", static_cast<int>(Account::Security_lock));

    if (!query.exec()) {
        qDebug() << "[Error] Insert into devices failed:" << query.lastError().text();
        return false;
    }
    qDebug() << "[Info] Insert into 'devices' succeeded!";
    return true;
}

// 拉取设备数据
QVariantList DataBase::pullDeviceData(const QString& username)
{
    if (!checkAndReconnect()) {
        return QVariantList();
    }
    QVariantList deviceList;
    QSqlQuery query(db);

    query.prepare("SELECT user_id FROM users WHERE account = :account");
    query.bindValue(":account", username);
    if (!query.exec()) {
        qDebug() << "[Error] Failed to get user_id:" << query.lastError().text();
        return deviceList;
    }

    int userId = -1;
    if (query.next()) {
        userId = query.value("user_id").toInt();
        qDebug() << "[Info] Found user_id =" << userId;
    } else {
        qDebug() << "[Error] No user found for account:" << username;
        return deviceList;
    }

    query.prepare("SELECT device_name, ip_address, password FROM devices WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);
    if (!query.exec()) {
        qDebug() << "[Error] Failed to get device data:" << query.lastError().text();
        return deviceList;
    }

    while (query.next()) {
        QVariantMap device;
        device["deviceName"] = query.value("device_name").toString();
        device["deviceIP"] = query.value("ip_address").toString();
        device["account"] = username;
        device["password"] = query.value("password").toString();
        device["isConnected"] = false;
        device["extended"] = false;
        deviceList.append(device);
    }

    qDebug() << "[Info] Fetched" << deviceList.size() << "devices for user" << username;
    return deviceList;
}

// 删除设备数据
bool DataBase::deleteDeviceData(const QString& ip_address)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("DELETE FROM devices WHERE ip_address = :ip_address");
    query.bindValue(":ip_address", ip_address);

    if (!query.exec()) {
        qDebug() << "[Error] Failed to delete device data:" << query.lastError().text();
        return false;
    }
    return true;
}

// 修改密码
bool DataBase::changePassword(const QString& oldPassword, const QString& newPassword)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("SELECT password FROM users WHERE account = :userName");
    query.bindValue(":userName", Account::Remote_username);

    if (!query.exec()) {
        qDebug() << "[Error] Failed to fetch current password:" << query.lastError().text();
        return false;
    }

    if (!query.next()) {
        qDebug() << "[Error] User not found!";
        return false;
    }

    QString currentPassword = query.value(0).toString();
    if (currentPassword != oldPassword) {
        qDebug() << "[Error] Old password is incorrect!";
        return false;
    }

    query.prepare("UPDATE users SET password = :newPassword WHERE account = :userName");
    query.bindValue(":newPassword", newPassword);
    query.bindValue(":userName", Account::Remote_username);

    if (!query.exec()) {
        qDebug() << "[Error] Failed to update password:" << query.lastError().text();
        return false;
    }
    qDebug() << "Password updated successfully!";
    return true;
}

// 保存新用户名
bool DataBase::saveNewUsername(const QString& name)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("UPDATE users SET nickname = :nickname WHERE account = :account");
    query.bindValue(":nickname", name);
    query.bindValue(":account", Account::Remote_username);

    if (!query.exec()) {
        qDebug() << "[Error] Failed to update username:" << query.lastError().text();
        return false;
    }
    qDebug() << "Username updated successfully!";
    return true;
}

// 加载用户名
QString DataBase::loadUsername()
{
    if (!checkAndReconnect()) {
        return QString();
    }
    QSqlQuery query(db);
    query.prepare("SELECT nickname FROM users WHERE account = :account");
    query.bindValue(":account", Account::Remote_username);

    if (!query.exec()) {
        qDebug() << "[Error] Failed to load username:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    }
    qDebug() << "[Error] User not found or no nickname set!";
    return QString();
}

// 保存安全锁密码
bool DataBase::saveSecurityLockPassword(const QString &oldPassword, const QString &newPassword)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("SELECT securitylock FROM users WHERE account = :account");
    query.bindValue(":account", Account::Remote_username);

    if (!query.exec()) {
        logger.print("Database", "Failed to query securitylock: " + query.lastError().text());
        return false;
    }

    if (!query.next()) {
        logger.print("Database", "User not found for account: " + Account::Remote_username);
        return false;
    }

    QString currentPassword = query.value("securitylock").toString();
    if (currentPassword.isEmpty()) {
        query.prepare("UPDATE users SET securitylock = :securitylock WHERE account = :account");
        query.bindValue(":securitylock", newPassword);
        query.bindValue(":account", Account::Remote_username);
        if (!query.exec()) {
            logger.print("Database", "Failed to set initial securitylock: " + query.lastError().text());
            return false;
        }
        logger.print("Database", "Initial security lock password set successfully!");
        return true;
    }

    if (currentPassword != oldPassword) {
        logger.print("Database", "Old security lock password is incorrect");
        return false;
    }

    query.prepare("UPDATE users SET securitylock = :securitylock WHERE account = :account");
    query.bindValue(":securitylock", newPassword);
    query.bindValue(":account", Account::Remote_username);
    if (!query.exec()) {
        logger.print("Database", "Failed to update securitylock: " + query.lastError().text());
        return false;
    }
    logger.print("Database", "Updated user's security lock password successfully!");
    return true;
}

// 检查安全锁密码
bool DataBase::checkSecurityLockPassword(const QString &inputpassword)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("SELECT securitylock FROM users WHERE account = :account");
    query.bindValue(":account", Account::Remote_username);

    if (!query.exec()) {
        logger.print("Database", "Failed to load securitylockpassword:" + query.lastError().text());
        return false;
    }
    if (query.next()) {
        return query.value(0).toString() == inputpassword;
    }
    return false;
}

// 检查是否启用安全锁
bool DataBase::isSecurityLocked(const QString &account)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("SELECT isLocked FROM users WHERE account = :account LIMIT 1");
    query.bindValue(":account", account);

    if (!query.exec()) {
        logger.print("Database", "Failed to load isLocked flag: " + query.lastError().text());
        return false;
    }

    if (query.next()) {
        int locked = query.value(0).toInt();
        return (locked != 0);
    }
    logger.print("Database", "User not found: " + account);
    return false;
}

// 保存安全锁状态
bool DataBase::saveSecurityLock(const QString &account, bool isOpened)
{
    if (!checkAndReconnect()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("UPDATE users SET isLocked = :locked WHERE account = :account LIMIT 1");
    query.bindValue(":locked", isOpened ? 1 : 0);
    query.bindValue(":account", account);

    if (!query.exec()) {
        logger.print("Database", "Failed to update isLocked flag: " + query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() == 1) {
        return true;
    } else {
        logger.print("Database", "No rows updated for account: " + account);
        return false;
    }
}
