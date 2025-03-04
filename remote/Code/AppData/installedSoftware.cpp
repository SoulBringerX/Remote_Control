#ifdef WIN32

#include "installedsoftware.h"
#include <QVariantList>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <shlwapi.h>
#include <QDesktopServices>
#include <QUrl>


InstalledSoftware::InstalledSoftware(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<SoftwareInfo>("SoftwareInfo");
}

QVariantList InstalledSoftware::softwareList() const {
    return m_softwareList;
}

QStringList InstalledSoftware::getLocalIPs() const {
    return getAllLocalIPs();
}

void InstalledSoftware::refreshSoftwareList() {
    m_softwareList.clear();

    // 获取本机IP地址（提前获取避免循环内重复调用）
    const QStringList localIPs = getAllLocalIPs();

    // 获取桌面路径
    const QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    // 获取注册表路径
    const QStringList regPaths = {
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
    };

    // 获取桌面快捷方式
    QStringList desktopShortcuts;
    if (QDir(desktopPath).exists()) {
        const QStringList shortcutFiles = QDir(desktopPath).entryList(QStringList() << "*.lnk", QDir::Files);
        for (const QString& shortcutFile : shortcutFiles) {
            QFileInfo fileInfo(desktopPath + "/" + shortcutFile); // 直接获取文件信息
            if (fileInfo.exists() && fileInfo.isFile()) {
                desktopShortcuts.append(fileInfo.absoluteFilePath());
            }
        }
    }

    // 遍历注册表路径
    for (const QString& regPath : regPaths) {
        QSettings settings(regPath, QSettings::NativeFormat);

        for (const QString& subKey : settings.childGroups()) {
            settings.beginGroup(subKey);

            SoftwareInfo info;
            info.name = settings.value("DisplayName").toString();
            info.version = settings.value("DisplayVersion").toString();
            info.installLocation = settings.value("InstallLocation").toString();
            info.publisher = settings.value("Publisher").toString();
            info.uninstallExePath = settings.value("UninstallString").toString();
            info.installDate = settings.value("InstallDate").toString();
            info.localIPs = localIPs; // 绑定IP信息

            // 查找主程序路径
            info.mainExePath = findMainExecutable(info.installLocation);

            // 过滤有效条目
            if (!info.name.isEmpty() &&
                !info.uninstallExePath.isEmpty() &&
                !info.mainExePath.isEmpty())
            {
                QVariantMap entry;
                entry["name"] = info.name;
                entry["version"] = info.version;
                entry["installLocation"] = info.installLocation;
                entry["mainExe"] = info.mainExePath;
                entry["uninstallExe"] = info.uninstallExePath;
                entry["publisher"] = info.publisher;
                entry["installDate"] = info.installDate;
                entry["localIPs"] = info.localIPs;

                m_softwareList.append(entry);
            }

            settings.endGroup();
        }
    }

    // 遍历桌面快捷方式
    for (const QString& shortcut : desktopShortcuts) {
        QFileInfo fileInfo(shortcut);
        if (fileInfo.exists() && fileInfo.isFile()) {
            // 解析快捷方式目标
            QString targetPath = parseShortcutTarget(shortcut);
            if (!targetPath.isEmpty()) {
                SoftwareInfo info;
                info.name = fileInfo.baseName();
                info.mainExePath = targetPath;
                info.uninstallExePath = findUninstaller(targetPath);
                info.localIPs = localIPs; // 绑定IP信息

                // 过滤有效条目
                if (!info.name.isEmpty() &&
                    !info.uninstallExePath.isEmpty() &&
                    !info.mainExePath.isEmpty())
                {
                    QVariantMap entry;
                    entry["name"] = info.name;
                    entry["mainExe"] = info.mainExePath;
                    entry["uninstallExe"] = info.uninstallExePath;
                    entry["localIPs"] = info.localIPs;

                    m_softwareList.append(entry);
                }
            }
        }
    }

    emit softwareListChanged();
}

// 解析快捷方式目标
QString InstalledSoftware::parseShortcutTarget(const QString& shortcutPath) const {
    QSettings shortcut(shortcutPath, QSettings::IniFormat);
    QString target = shortcut.value("Shell\\Open\\Command").toString();
    if (target.startsWith("\"")) {
        target = target.mid(1, target.indexOf("\"", 1) - 1);
    }
    return target;
}

// 查找卸载程序路径
QString InstalledSoftware::findUninstaller(const QString& exePath) const {
    QString uninstallPath;
    QFileInfo fileInfo(exePath);
    QString dirPath = fileInfo.absolutePath();

    // 检查卸载程序
    QStringList uninstallExes = {"uninstall.exe", "uninstaller.exe", "uninstall.bat", "uninstaller.bat"};
    for (const QString& uninstallExe : uninstallExes) {
        QString path = QDir(dirPath).filePath(uninstallExe);
        if (QFile::exists(path)) {
            uninstallPath = path;
            break;
        }
    }

    // 如果未找到，检查上级目录
    if (uninstallPath.isEmpty()) {
        QString parentDir = QFileInfo(dirPath).absolutePath(); // 获取上级目录
        for (const QString& uninstallExe : uninstallExes) {
            QString path = QDir(parentDir).filePath(uninstallExe);
            if (QFile::exists(path)) {
                uninstallPath = path;
                break;
            }
        }
    }

    return uninstallPath;
}

QString InstalledSoftware::findMainExecutable(const QString& installPath) const {
    if (installPath.isEmpty()) return "";

    QDir dir(installPath);
    if (!dir.exists()) return "";

    // 优先查找与目录同名的exe
    QString dirName = dir.dirName();
    QString possibleMainExe = dir.filePath(dirName + ".exe");
    if (QFile::exists(possibleMainExe)) {
        return possibleMainExe;
    }

    // 查找其他exe文件
    QStringList exeFiles = dir.entryList(QStringList() << "*.exe", QDir::Files);
    if (!exeFiles.isEmpty()) {
        return dir.filePath(exeFiles.first());
    }

    return "";
}

QStringList InstalledSoftware::getAllLocalIPs() const {
    QStringList ips;
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    for (const QNetworkInterface& qinterface : interfaces) {
        // Skip loopback interfaces (e.g., 127.0.0.1)
        if (qinterface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            continue;
        }

        // Skip interfaces that are not active
        if (!qinterface.flags().testFlag(QNetworkInterface::IsUp)) {
            continue;
        }

        // Iterate through the address entries of the interface
        for (const QNetworkAddressEntry& entry : qinterface.addressEntries()) {
            const QHostAddress& ip = entry.ip();

            // Check if the address is an IPv4 address
            if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
                ips.append(ip.toString());
            }
        }
    }

    return ips;
}
#endif
