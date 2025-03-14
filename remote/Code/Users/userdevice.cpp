#ifdef LINUX
#include "userdevice.h"
UserDevice::UserDevice(QObject *parent)
    : QObject(parent)
{
    // 如果需要在构造时初始化某些东西，可以在这里处理
}
void UserDevice::sendUserDevice(const QString& hostname, const QString& username, const QString& password)
{
    qDebug() << "sendUserDevice 被调用, 参数:" << hostname << username << password;

    if(Account::isOnline == true)
    {
        logger.print("RemoteRDP","用户设备数据传输");
        DataBase db;
        if(db.pushDeviceData(hostname,username,password)) {
            logger.print("RemoteRDP","添加设备成功");
        }
        else {
            logger.print("RemoteRDP","添加设备失败");
        }
    }
}
QVariantList UserDevice::getUserDevices()
{
    DataBase db;
    if(Account::isOnline == true)
        return db.pullDeviceData(Account::Remote_username);
    if(Account::isOnline == false)
        return db.pullDeviceData("root");
}
void UserDevice::deleteUserDevice(const QString& hostname)
{
    DataBase db;
    if(Account::isOnline == true)
    {
        if(db.deleteDeviceData(hostname))
            logger.print("RemoteRDP","删除设备成功");
        else
            logger.print("RemoteRDP","删除设备失败");
    }
}
#endif
