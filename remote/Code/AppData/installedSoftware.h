#ifndef INSTALLEDSOFTWARE_H
#define INSTALLEDSOFTWARE_H

#ifdef WIN32

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
#include <shlobj.h>
#include <objidl.h>
#include <iostream>
#include <string>
#include <vector>
#include <shellapi.h>
#include <shlwapi.h>
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")

// 定义一个结构体，保存软件信息
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

    SoftwareInfo()
        : name(), version(), installLocation(), mainExePath(),
        uninstallExePath(), publisher(), installDate(), localIPs(), iconPath() {}
};

Q_DECLARE_METATYPE(SoftwareInfo)

class InstalledSoftware : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList softwareList READ softwareList NOTIFY softwareListChanged)

public:
    explicit InstalledSoftware(QObject *parent = nullptr);

    // 获取已安装软件列表
    QVariantList softwareList() const;

    // 获取本机IP地址列表
    Q_INVOKABLE QStringList getLocalIPs() const;

    // 查找卸载程序
    QString findUninstaller(const QString &exePath) const;

    // 解析快捷方式目标路径
    QString parseShortcutTarget(const QString &shortcutPath) const;

    // 查找注册表路径
    QString findRegistryPath(const QString &exePath) const;

    // 查找主程序路径
    QString findMainExecutable(const QString &installPath) const;

    // 查找图标路径
    QString findIconPath(const QString &exePath) const;

    // 获取所有本地IP地址
    QStringList getAllLocalIPs() const;

    // 获取快捷方式目标路径
    QString getShellPath(const QString &shortcutPath);

    // 保存软件信息到注册表
    Q_INVOKABLE bool saveSoftwareInfo(const SoftwareInfo &info);

    // 从注册表删除软件信息
    Q_INVOKABLE bool deleteSoftwareInfo(const SoftwareInfo &info);

    // 存储软件列表
    QVariantList m_softwareList;

public slots:
    // 刷新软件列表
    void refreshSoftwareList();

signals:
    // 软件列表改变的信号
    void softwareListChanged();
};

#endif

#endif // INSTALLEDSOFTWARE_H
