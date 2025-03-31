#ifndef USERDEVICE_H
#define USERDEVICE_H
#ifdef LINUX
#include <QString>
#include <QObject>
#include "./Code/DataBase/database.h"
#include "../LogUntils/AppLog.h"
class UserDevice : public QObject
{
    Q_OBJECT
public:
    explicit UserDevice(QObject *parent = nullptr);
    //这里是和远端的数据库中的用户设备管理
    Q_INVOKABLE void sendUserDevice(const QString& hostname, const QString& username, const QString& password);
    Q_INVOKABLE QVariantList getUserDevices();
    Q_INVOKABLE void deleteUserDevice(const QString& hostname);
    Q_INVOKABLE void saveAppsToDevice(const QString &ip, const QVariant &apps);
    Q_INVOKABLE void uninstallApp(const QString &ip, const QString &appName);
    Q_INVOKABLE QVariantList loadAppfromini(const QString &ip);
    // 安装包管理相关
    Q_INVOKABLE QVariantMap getInstallPackageInfo(const QString &filePath);
};
#endif
#endif // USERDEVICE_H
