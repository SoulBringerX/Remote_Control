#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include "../LogUntils/AppLog.h"

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

    // 远端、本地App数据
    bool pushApplicationData();

    bool fetchApplicationData();

private:
    QSqlDatabase db;
    static DataBase* instance;
    static QMutex mutex; // 静态成员变量声明
};

#endif // DATABASE_H
