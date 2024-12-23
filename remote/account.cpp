#include "account.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QQmlContext>
#include <QDir>

Account::Account(QObject *parent)
    : QObject(parent), m_userName(""), m_passWord(""), m_userIconPath("")
{
}

// 获取用户名
QString Account::userName() const
{
    return m_userName;
}

// 设置用户名
void Account::setUserName(const QString &userName)
{
    if (m_userName != userName) {
        m_userName = userName;
        emit userNameChanged();
    }
}


// 获取密码
QString Account::passWord() const
{
    return m_passWord;
}

// 设置密码
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

// 设置用户图像路径（换用图像二进制文件输入数据库）
// void Account::setUserIconPath(const QString &path)
// {
//     if (m_userIconPath != path) {
//         m_userIconPath = path;
//     }
// }

// 登录检查
bool Account::loginCheck(const QString &userName, const QString &passWord)
{
    qDebug()<< userName;
    qDebug()<< passWord;

    if (userName.isEmpty() || passWord.isEmpty()) {
        return false;
    }
    DataBase db;
    if(db.isLogin(userName,passWord))
        return true;
    else
        return false;
    return true;
}

// 注册检查
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
    // 这里可以添加一些代码来处理用户名改变时的改变头像的逻辑（难度有点大，待后期实现）
}

void openFileManager() 
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::home().absolutePath()));
}