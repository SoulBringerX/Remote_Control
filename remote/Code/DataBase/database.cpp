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
bool DataBase::pushDeviceData(const QString& hostname,
                                   const QString& username,
                                   const QString& password)
{
    // 确保 db 已经用 QSqlDatabase::addDatabase() 并且 db.open() 成功
    if (!db.isOpen()) {
        qDebug() << "[Error] Database not open!";
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
        // 如果查不到对应用户，可能需要给出提示或自行处理
        qDebug() << "[Error] No user found for account:" << Account::Remote_username;
        return false;
    }

    // 2) 在 device 表中插入记录（使用上面查询到的 userId）
    query.prepare(
        "INSERT INTO devices (user_id, device_name, ip_address, password, last_connected, is_trusted) "
        "VALUES (:user_id, :device_name, :ip_address, :password, :last_connected, :is_trusted)"
    );

    // 绑定参数
    query.bindValue(":user_id", userId);
    query.bindValue(":device_name", username);
    query.bindValue(":ip_address", hostname);
    query.bindValue(":password", password);
    query.bindValue(":last_connected", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")); // 转换为字符串
    query.bindValue(":is_trusted", static_cast<int>(Account::Security_lock)); // 确保类型匹配

    if (!query.exec()) {
        qDebug() << "[Error] Insert into devices failed:" << query.lastError().text();
        return false;
    }

    qDebug() << "[Info] Insert into 'devices' succeeded!";
    return true;


    if (!query.exec()) {
        qDebug() << "[Error] Insert into device failed:" << query.lastError().text();
        return false;
    }

    qDebug() << "[Info] Insert into 'device' succeeded!";
    return true;
}
QVariantList DataBase::pullDeviceData(const QString& username)
{
    QVariantList deviceList;  // 存储查询到的设备信息

    // 确保数据库已打开
    if (!db.isOpen()) {
        qDebug() << "[Error] Database not open!";
        return deviceList;
    }

    QSqlQuery query(db);

    // 1️⃣ 先查询 user_id
    query.prepare("SELECT user_id FROM users WHERE account = :account");
    query.bindValue(":account", username);  // 使用 username 参数，而不是 Account::Remote_username

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

    // 2️⃣ 查询该用户关联的设备
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
        device["account"] = query.value("device_name").toString();  // 直接使用传入的 username
        device["password"] = query.value("password").toString();
        device["isConnected"] = false;
        device["extended"] = false;

        deviceList.append(device);  // 添加到列表
    }

    qDebug() << "[Info] Fetched" << deviceList.size() << "devices for user" << username;
    return deviceList;  // 返回设备数据列表
}
bool DataBase::deleteDeviceData(const QString& ip_address)
{
    // 确保数据库已打开
    if (!db.isOpen()) {
        qDebug() << "[Error] Database not open!";
        return false;
    }

    QSqlQuery query(db);
    // 删除数据库的设备数据
    query.prepare("DELETE FROM devices WHERE ip_address = :ip_address");
    query.bindValue(":ip_address", ip_address); // 修正绑定的键

    if (!query.exec()) {
        qDebug() << "[Error] Failed to delete device data:" << query.lastError().text();
        return false;
    }

    return true;
}
bool DataBase::changePassword(const QString& oldPassword, const QString& newPassword)
{
    // 确保数据库已打开
    if (!db.isOpen()) {
        qDebug() << "[Error] Database not open!";
        return false;
    }

    // 第一步：验证旧密码是否正确
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

    // 第二步：更新密码
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

bool DataBase::saveNewUsername(const QString& name)
{
    // 确保数据库已打开
    if (!db.isOpen()) {
        qDebug() << "[Error] Database not open!";
        return false;
    }

    QSqlQuery query(db);
    // 更新用户的昵称
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
QString DataBase::loadUsername()
{
    // 确保数据库已打开
    if (!db.isOpen()) {
        qDebug() << "[Error] Database not open!";
        return QString();
    }

    QSqlQuery query(db);
    // 查询用户的昵称
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
