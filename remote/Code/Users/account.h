#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QObject>
#include "./Code/DataBase/database.h"

class Account : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString passWord READ passWord WRITE setPassWord)

public:
    explicit Account(QObject *parent = nullptr);
    // 获得和设置userName和passWord
    QString userName() const;
    void setUserName(const QString &userName);

    QString passWord() const;
    void setPassWord(const QString &passWord);


    // 用户帐号密码的查找机制
    Q_INVOKABLE bool loginCheck(const QString &userName, const QString &passWord);
    Q_INVOKABLE bool registerCheck(const QString &userName, const QString &passWord);
    Q_INVOKABLE bool userRegister(const QString &userName, const QString &passWord);
    Q_INVOKABLE void openFileManager();

signals:
    void userNameChanged();

private:
    QString m_userName;
    QString m_passWord;
    QString m_userIconPath;
};

#endif // ACCOUNT_H
