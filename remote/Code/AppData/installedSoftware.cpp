#include "installedsoftware.h"
#include <QVariantList>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <shlwapi.h>
#include <QDesktopServices>
#include <QUrl>
#include <QFileIconProvider>
#include <QBuffer>

InstalledSoftware::InstalledSoftware(QObject *parent)
    : QObject(parent) {
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

    // 获取注册表路径
    const QStringList regPaths = {
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
    };

    // 遍历注册表路径
    for (const QString &regPath : regPaths) {
        QSettings settings(regPath, QSettings::NativeFormat);

        for (const QString &subKey : settings.childGroups()) {
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

            // 获取图标路径
            info.iconPath = findIconPath(info.mainExePath);

            // 过滤有效条目
            if (!info.name.isEmpty() &&
                !info.uninstallExePath.isEmpty() &&
                !info.mainExePath.isEmpty()) {
                QVariantMap entry;
                entry["name"] = info.name;
                entry["version"] = info.version;
                entry["installLocation"] = info.installLocation;
                entry["mainExe"] = info.mainExePath;
                entry["uninstallExe"] = info.uninstallExePath;
                entry["publisher"] = info.publisher;
                entry["installDate"] = info.installDate;
                entry["localIPs"] = info.localIPs;
                entry["iconPath"] = info.iconPath;

                m_softwareList.append(entry);
            }

            settings.endGroup();
        }
    }

    emit softwareListChanged();
}
// 获取图标路径
QString InstalledSoftware::findIconPath(const QString &exePath) const {
    QFileIconProvider iconProvider;
    QFileInfo fileInfo(exePath);
    QIcon icon = iconProvider.icon(fileInfo);
    QPixmap pixmap = icon.pixmap(32, 32);
    QImage image = pixmap.toImage();

    if (image.isNull()) {
        return QString();
    }

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG"); // 保存为 PNG 格式
    return "data:image/png;base64," + byteArray.toBase64();
}

// 解析快捷方式目标
QString InstalledSoftware::parseShortcutTarget(const QString &shortcutPath) const {
    QSettings shortcut(shortcutPath, QSettings::IniFormat);
    QString target = shortcut.value("Shell\\Open\\Command").toString();
    if (target.startsWith("\"")) {
        target = target.mid(1, target.indexOf("\"", 1) - 1);
    }
    return target;
}

// 查找卸载程序路径
QString InstalledSoftware::findUninstaller(const QString &exePath) const {
    QString uninstallPath;
    QFileInfo fileInfo(exePath);
    QString dirPath = fileInfo.absolutePath();

    // 检查卸载程序
    QStringList uninstallExes = {"uninstall.exe", "uninstaller.exe", "uninstall.bat", "uninstaller.bat"};
    for (const QString &uninstallExe : uninstallExes) {
        QString path = QDir(dirPath).filePath(uninstallExe);
        if (QFile::exists(path)) {
            uninstallPath = path;
            break;
        }
    }

    // 如果未找到，检查下一级目录
    if (uninstallPath.isEmpty()) {
        QString parentDir = QFileInfo(dirPath).absolutePath(); // 获取上级目录
        for (const QString &uninstallExe : uninstallExes) {
            QString path = QDir(parentDir).filePath(uninstallExe);
            if (QFile::exists(path)) {
                uninstallPath = path;
                break;
            }
        }
    }

    // 如果仍未找到，检查更深层次的目录
    if (uninstallPath.isEmpty()) {
        QDir dir(dirPath);
        QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &subDir : subDirs) {
            QString subDirPath = QDir(dirPath).filePath(subDir);
            for (const QString &uninstallExe : uninstallExes) {
                QString path = QDir(subDirPath).filePath(uninstallExe);
                if (QFile::exists(path)) {
                    uninstallPath = path;
                    break;
                }
            }
            if (!uninstallPath.isEmpty()) {
                break;
            }
        }
    }

    return uninstallPath;
}

// 查找注册表路径
QString InstalledSoftware::findRegistryPath(const QString &exePath) const {
    QFileInfo fileInfo(exePath);
    QString dirPath = fileInfo.absolutePath();

    // 遍历注册表路径
    const QStringList regPaths = {
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
    };

    for (const QString &regPath : regPaths) {
        QSettings settings(regPath, QSettings::NativeFormat);

        for (const QString &subKey : settings.childGroups()) {
            settings.beginGroup(subKey);

            QString installLocation = settings.value("InstallLocation").toString();
            if (installLocation == dirPath) {
                return regPath + "\\" + subKey;
            }

            settings.endGroup();
        }
    }

    return QString();
}

// 查找主程序路径
QString InstalledSoftware::findMainExecutable(const QString &installPath) const {
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

// 获取本机IP地址
QStringList InstalledSoftware::getAllLocalIPs() const {
    QStringList ips;
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    for (const QNetworkInterface &qinterface : interfaces) {
        // 跳过回环接口（例如：127.0.0.1）
        if (qinterface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            continue;
        }

        // 跳过未激活的接口
        if (!qinterface.flags().testFlag(QNetworkInterface::IsUp)) {
            continue;
        }

        // 遍历接口的地址条目
        for (const QNetworkAddressEntry &entry : qinterface.addressEntries()) {
            const QHostAddress &ip = entry.ip();

            // 检查是否为IPv4地址
            if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
                ips.append(ip.toString());
            }
        }
    }

    return ips;
}
