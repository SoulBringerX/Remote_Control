#ifndef USERDEVICE_H
#define USERDEVICE_H

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
};

#endif // USERDEVICE_H
