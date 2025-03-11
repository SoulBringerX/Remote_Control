#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>
#include <QMutex>
#include "../LogUntils/AppLog.h"
#include "../Users/account.h"
#include "../AppData/devicedate.h"
class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);

    ~DataBase();

    static DataBase* getInstance();
    // 用户账号密码
    bool isLogin(const QString &userName, const QString &passWord);
    bool isRegister(const QString &userName, const QString &passWord);
    bool userRegister(const QString &userName, const QString &passWord);

    // 用户远端设备数据
    bool pushDeviceData(const QString& hostname, const QString& username, const QString& password);
    QVariantList pullDeviceData(const QString& username);
private:
    QSqlDatabase db;
    static DataBase* instance;
    static QMutex mutex; // 静态成员变量声明
};

#endif // DATABASE_H
