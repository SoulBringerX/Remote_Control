#include "account.h"

Account::Account(QObject *parent)
    : QObject(parent), m_userName(""), m_passWord(""), m_userIconPath("")
{
}

QString Account::userName() const
{
    return m_userName;
}

void Account::setUserName(const QString &userName)
{
    if (m_userName != userName) {
        m_userName = userName;
        emit userNameChanged();
    }
}

QString Account::passWord() const
{
    return m_passWord;
}

void Account::setPassWord(const QString &passWord)
{
    if (m_passWord != passWord) {
        m_passWord = passWord;
    }
}

QString Account::userIconPath() const
{
    return m_userIconPath;
}

void Account::setUserIconPath(const QString &path)
{
    if (m_userIconPath != path) {
        m_userIconPath = path;
    }
}

bool Account::loginCheck(const QString &userName, const QString &passWord)
{
    if (userName.isEmpty() || passWord.isEmpty()) {
        return false;
    }
    // 连接远端数据库进行数据查看去根据用户的账户密码来确定用户是否存在
    
    // if (userName == m_userName && passWord == m_passWord) {
    //     return true;
    // }
    return true;
}

bool Account::registerCheck(const QString &userName, const QString &passWord)
{
    if (userName.isEmpty() || passWord.isEmpty()) {
        return false;
    }
    return true;
}

// signals
void userNameChanged()
{
    // 这里可以添加一些代码来处理用户名改变时的逻辑
}
