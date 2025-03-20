#ifdef LINUX
#include "userdevice.h"
#include <QString>
#include <QVariant>
#include <QList>
#include <QVariantMap>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QJsonArray>
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
void UserDevice::saveAppsToDevice(const QString &ip, const QVariant &apps) {
    QList<QVariantMap> appList = apps.value<QList<QVariantMap>>();
    qDebug() << "Saving apps to device:" << ip;
    qDebug() << "Apps to save:" << appList;

    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configPath);
    QSettings settings(configPath + "/" + ip + ".ini", QSettings::IniFormat);

    settings.beginGroup("Apps");
    QStringList groups = settings.childGroups();
    settings.endGroup();

    QSet<QString> existingApps;
    for (const QString &group : groups) {
        settings.beginGroup("Apps/" + group);
        QString appName = settings.value("AppName").toString();
        settings.endGroup();
        existingApps.insert(appName);
    }

    settings.beginGroup("Apps");
    settings.remove("");
    settings.endGroup();

    for (int i = 0; i < appList.size(); ++i) {
        const QVariantMap &app = appList[i];
        QString appName = app["AppName"].toString();

        if (existingApps.contains(appName)) {
            qDebug() << "App already exists, skipping:" << appName;
            continue;
        }

        settings.beginGroup("Apps");
        settings.beginGroup(QString::number(i));
        settings.setValue("AppName", appName);
        settings.setValue("RdpAppName", app["RdpAppName"].toString());
        settings.setValue("AppIconPath", app["AppIconPath"].toString());
        settings.endGroup();
        settings.endGroup();

        qDebug() << "App added:" << appName;

        existingApps.insert(appName);
    }

    qDebug() << "Saving apps to device:" << ip;
}

void UserDevice::uninstallApp(const QString &ip, const QString &appName) {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QSettings settings(configPath + "/" + ip + ".ini", QSettings::IniFormat);

    settings.beginGroup("Apps");
    QStringList groups = settings.childGroups();
    settings.endGroup();

    for (const QString &group : groups) {
        settings.beginGroup("Apps/" + group);
        QString currentAppName = settings.value("AppName").toString();
        settings.endGroup();

        if (currentAppName == appName) {
            settings.beginGroup("Apps");
            settings.remove(group);
            settings.endGroup();
            break;
        }
    }

    qDebug() << "Uninstalling app:" << appName << "from device:" << ip;
}


QVariantList UserDevice::loadAppfromini(const QString &ip)
{
    QVariantList appList;
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QSettings settings(configPath + "/" + ip + ".ini", QSettings::IniFormat);

    settings.beginGroup("Apps");
    QStringList groups = settings.childGroups();
    settings.endGroup();

    for (const QString &group : groups) {
        settings.beginGroup("Apps/" + group);
        QVariantMap app;
        app["AppName"] = settings.value("AppName").toString();
        app["RdpAppName"] = settings.value("RdpAppName").toString();
        app["AppIconPath"] = settings.value("AppIconPath").toString();
        settings.endGroup();
        appList.append(app);
    }

    qDebug() << "Loaded apps from device:" << ip;
    qDebug() << "Apps:" << appList;

    return appList;
}
#endif
