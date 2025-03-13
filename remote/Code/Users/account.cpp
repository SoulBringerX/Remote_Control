#include "account.h"
#include <QDebug>
#include <QUrl>
#include <QQmlContext>
#include <QDir>
#include <QImage>
#include <QFileDialog>
#include "../LogUntils/AppLog.h"

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

// 登录检查
bool Account::loginCheck(const QString &userName, const QString &passWord)
{
    logger.print("Accont 用户:",userName);

    if (userName.isEmpty() || passWord.isEmpty()) {
        return false;
    }
    if(userName == "user"&&passWord == "123456")
    {
        logger.print("Accont 用户:","本地用户登录");
        // 本地用用户登入
        return true;
    }
    DataBase db;
    if(db.isLogin(userName,passWord))
    {
        Account::isOnline = true;
        return true;
    }
    else
        return false;
    return true;
}

// 注册检查
bool Account::registerCheck(const QString &userName, const QString &passWord)
{
    if (DataBase::getInstance()->isRegister(userName, passWord) == false) {
        logger.print("Account 注册检查","该账户已存在");
        return false;
    }
    return true;
}

//用户检查
bool Account::userRegister(const QString &userName, const QString &passWord) {
    return DataBase::getInstance()->userRegister(userName, passWord);
}

void Account::openFileManager()
{
    QString fileName = QFileDialog::getOpenFileName(
        nullptr,
        tr("选择头像"),
        QDir::homePath(),
        tr("Image Files (*.png *.jpg *.jpeg *.bmp)")
        );

    if (!fileName.isEmpty())
    {
        QImage image(fileName);
        if (!image.isNull())
        {
            // 处理图片，例如调整大小或裁剪
            logger.print("Class Account-选中图片文件：",fileName);
            // 分析图片
        }
        else
        {
            logger.print("Class Account","选中图片失败");
        }
    }
}

bool Account::changePassWord(const QString& oldPassword,const QString& newPassword)
{
    if(Account::isOnline)
        return DataBase::getInstance()->changePassword(oldPassword,newPassword);
}

bool Account::saveNewUsername(const QString& name)
{
    if(Account::isOnline)
        return DataBase::getInstance()->saveNewUsername(name);
}
QString Account::loadUsername()
{
    if(Account::isOnline)
        return DataBase::getInstance()->loadUsername();
}
