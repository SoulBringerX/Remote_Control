#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);

    ~DataBase();

    bool isLogin(const QString &userName, const QString &passWord);

    bool isRegister(const QString &userName, const QString &passWord);

private:
    QSqlDatabase db;
};

#endif // DATABASE_H
