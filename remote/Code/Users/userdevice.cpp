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
    QVariantList list = apps.toList();
    QList<QVariantMap> appList;
    for (const QVariant &item : list) {
        appList.append(item.toMap());
    }

    qDebug() << "Saving apps to device:" << ip;
    qDebug() << "Apps to save:" << appList;

    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configPath);
    QSettings settings(configPath + "/" + ip + ".ini", QSettings::IniFormat);

    // **清空旧数据**
    settings.beginGroup("Apps");
    settings.remove("");
    settings.endGroup();

    // **直接写入新数据**
    settings.beginGroup("Apps");
    for (int i = 0; i < appList.size(); ++i) {
        const QVariantMap &app = appList[i];
        QString appName = app["AppName"].toString();

        settings.beginGroup(QString::number(i));
        settings.setValue("AppName", appName);
        settings.setValue("RdpAppName", app["RdpAppName"].toString());
        settings.setValue("AppIconPath", app["AppIconPath"].toString());
        settings.endGroup();

        qDebug() << "App added:" << appName;
    }
    settings.endGroup();

    qDebug() << "Saving apps to device completed:" << ip;
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

    qDebug() << "Loading apps from device:" << ip;
    qDebug() << "Found groups:" << groups;

    if (groups.isEmpty()) {
        qDebug() << "No saved applications found!";
        return appList;
    }

    for (const QString &group : groups) {
        settings.beginGroup("Apps/" + group);
        QVariantMap app;
        app["AppName"] = settings.value("AppName").toString();
        app["RdpAppName"] = settings.value("RdpAppName").toString();
        app["AppIconPath"] = settings.value("AppIconPath").toString();
        settings.endGroup();
        appList.append(app);
    }

    qDebug() << "Loaded apps:" << appList;
    return appList;
}

// 获取安装包信息（供 QML 调用）
QVariantMap UserDevice::getInstallPackageInfo(const QString &filePath)
{
    qDebug() << "发送函数已被调用";

    // 如果路径不是绝对路径，添加前导 "/" 以将其视为根目录下的路径
    QString absolutePath = filePath;
    if (!QDir::isAbsolutePath(filePath)) {
        absolutePath = "/" + filePath;
    }

    qDebug() << "当前工作目录：" << QDir::currentPath();
    qDebug() << "传入路径：" << filePath;
    qDebug() << "修正后的绝对路径：" << absolutePath;

    // 使用清理后的绝对路径创建 QFileInfo 对象
    QFileInfo fileInfo(QDir::cleanPath(absolutePath));
    QVariantMap packageInfo;

    // 检查文件是否存在且是有效文件
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        qDebug() << "文件不存在或不是有效文件：" << absolutePath;
        packageInfo["filePath"] = "";
        packageInfo["fileName"] = "";
        packageInfo["fileSize"] = -1;
        return packageInfo;
    }

    // 文件存在，返回详细信息
    packageInfo["filePath"] = fileInfo.absoluteFilePath();
    packageInfo["fileName"] = fileInfo.fileName();
    packageInfo["fileSize"] = fileInfo.size();

    return packageInfo;
}


#endif
