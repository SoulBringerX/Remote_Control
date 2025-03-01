#ifdef WIN32
#ifndef INSTALLEDSOFTWARE_H
#define INSTALLEDSOFTWARE_H

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QSettings>
#include <QDir>
#include <QIcon>
#include <QBuffer>
#include <QNetworkInterface>
#include <QStandardPaths>
#include <shlwapi.h>
#include <windows.h>
#include "../LogUntils/AppLog.h"


struct SoftwareInfo {
    QString name;
    QString version;
    QString installLocation;
    QString mainExePath;      // 新增：主程序路径
    QString uninstallExePath; // 修改：直接存储卸载程序路径
    QString publisher;
    QString installDate;
    QStringList localIPs;     // 新增：本机IP地址列表
};

Q_DECLARE_METATYPE(SoftwareInfo)

class InstalledSoftware : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList softwareList READ softwareList NOTIFY softwareListChanged)

public:
    explicit InstalledSoftware(QObject *parent = nullptr);

    QVariantList softwareList() const;
    Q_INVOKABLE QStringList getLocalIPs() const; // 新增：获取本机IP的公开方法
    QString findUninstaller(const QString& exePath) const;
    QString parseShortcutTarget(const QString& shortcutPath) const;


public slots:
    void refreshSoftwareList();

signals:
    void softwareListChanged();

private:
    QVariantList m_softwareList;

    // 新增：获取安装目录中的主程序路径
    QString findMainExecutable(const QString& installPath) const;

    // 新增：获取本机所有IPv4地址
    QStringList getAllLocalIPs() const;
};
#endif // INSTALLEDSOFTWARE_H
#endif
