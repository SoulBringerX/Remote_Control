#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <QString>

class Account
{
public:
    Account();
private:
    QString username;
    QString password;
    //用户头像转码存入数据库
    QString usericon;
};

#endif // ACCOUNT_H
