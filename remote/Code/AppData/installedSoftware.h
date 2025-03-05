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
    QString mainExePath;      // 主程序路径
    QString uninstallExePath; // 卸载程序路径
    QString publisher;
    QString installDate;
    QStringList localIPs;     // 本机IP地址列表
    QString iconPath;         // 图标路径

    SoftwareInfo() : name(), version(), installLocation(), mainExePath(), uninstallExePath(), publisher(), installDate(), localIPs(), iconPath() {}
};

Q_DECLARE_METATYPE(SoftwareInfo)

class InstalledSoftware : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList softwareList READ softwareList NOTIFY softwareListChanged)

public:
    explicit InstalledSoftware(QObject *parent = nullptr);

    QVariantList softwareList() const;
    Q_INVOKABLE QStringList getLocalIPs() const; // 获取本机IP的公开方法
    QString findUninstaller(const QString& exePath) const;
    QString parseShortcutTarget(const QString& shortcutPath) const;
    QString findRegistryPath(const QString& exePath) const;
    QString findMainExecutable(const QString& installPath) const;
    QString findIconPath(const QString& exePath) const;
    QStringList getAllLocalIPs() const;

    QVariantList m_softwareList;

public slots:
    void refreshSoftwareList();

signals:
    void softwareListChanged();
};

#endif // INSTALLEDSOFTWARE_H
