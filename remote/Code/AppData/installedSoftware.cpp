#include "installedSoftware.h"
#include <QVariantList>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QFileIconProvider>
#include <QBuffer>

#ifdef WIN32
#include <shlwapi.h>
#include <windows.h>
// 注意：注册表操作需要管理员权限

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
    const QStringList localIPs = getAllLocalIPs();

    QStringList desktopPaths = {
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
        "C:\\Users\\Public\\Desktop"
    };

    for (const QString &desktopPath : desktopPaths) {
        QDir desktopDir(desktopPath);
        if (!desktopDir.exists())
            continue;

        QStringList shortcuts = desktopDir.entryList(QStringList() << "*.lnk", QDir::Files);
        for (const QString &shortcut : shortcuts) {
            QString shortcutPath = desktopDir.filePath(shortcut);
            QString targetPath = getShellPath(shortcutPath);

            if (targetPath.isEmpty() || !QFile::exists(targetPath)) {
                qDebug() << "[Warning] Failed to resolve shortcut target:" << shortcutPath;
                continue;
            }

            // 输出软件名称和执行路径
            QFileInfo shortcutInfo(shortcutPath);
            QString softwareName = shortcutInfo.completeBaseName();
            qDebug() << "[Info] Found software:" << softwareName
                     << "Executable path:" << targetPath;

            SoftwareInfo info;
            info.name = softwareName;
            info.mainExePath = targetPath;
            info.installLocation = QFileInfo(targetPath).absolutePath();
            info.iconPath = findIconPath(targetPath);
            info.localIPs = localIPs;

            // 通过注册表查找卸载程序路径
            // 先查询64位应用信息，再查询32位应用信息（WOW6432Node）
            QString uninstallString;
            const QStringList registryPaths = {
                "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
                "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
            };

            for (const QString &regPath : registryPaths) {
                QSettings settings(regPath, QSettings::NativeFormat);
                QStringList subKeys = settings.childGroups();
                for (const QString &subKey : subKeys) {
                    settings.beginGroup(subKey);
                    QString displayName = settings.value("DisplayName").toString().trimmed();
                    QString tempUninstallString = settings.value("UninstallString").toString().trimmed();
                    settings.endGroup();

                    // 如果显示名称中包含软件名称，则认为匹配成功
                    if (!displayName.isEmpty() && displayName.contains(softwareName, Qt::CaseInsensitive)) {
                        uninstallString = tempUninstallString;
                        break;
                    }
                }
                if (!uninstallString.isEmpty()) {
                    break;
                }
            }
            info.uninstallExePath = uninstallString;

            // 扫描到软件后立即注册到注册表中
            bool regSuccess = saveSoftwareInfo(info);
            if (regSuccess) {
                qDebug() << "[Info] Successfully registered" << info.name << "to the registry.";
            } else {
                qDebug() << "[Error] Failed to register" << info.name << "to the registry.";
            }

            QVariantMap entry;
            entry["name"] = info.name;
            entry["installLocation"] = info.installLocation;
            entry["mainExe"] = info.mainExePath;
            entry["uninstallExe"] = info.uninstallExePath;
            entry["iconPath"] = info.iconPath;
            m_softwareList.append(entry);
        }
    }

    emit softwareListChanged();
}

QString InstalledSoftware::findIconPath(const QString &exePath) const {
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(QFileInfo(exePath));
    QPixmap pixmap = icon.pixmap(32, 32);

    if (pixmap.isNull()) {
        return QString();
    }

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.toImage().save(&buffer, "PNG");
    return "data:image/png;base64," + byteArray.toBase64();
}

QString InstalledSoftware::getShellPath(const QString &shortcutPath) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    IShellLinkW *psl = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IShellLinkW, (LPVOID*)&psl);
    if (FAILED(hr)) {
        CoUninitialize();
        return QString();
    }

    IPersistFile *ppf = nullptr;
    hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
    if (FAILED(hr)) {
        psl->Release();
        CoUninitialize();
        return QString();
    }

    hr = ppf->Load(shortcutPath.toStdWString().c_str(), STGM_READ);
    if (SUCCEEDED(hr)) {
        wchar_t shellPath[MAX_PATH] = {0};
        if (SUCCEEDED(psl->GetPath(shellPath, MAX_PATH, NULL, 0))) {
            ppf->Release();
            psl->Release();
            CoUninitialize();
            return QString::fromWCharArray(shellPath);
        }
    }

    ppf->Release();
    psl->Release();
    CoUninitialize();
    return QString();
}

QString InstalledSoftware::findUninstaller(const QString &installPath) const {
    if (installPath.isEmpty()) return "";

    static const QStringList uninstallExes = {
        "uninstall.exe", "uninstaller.exe", "setup.exe",
        "uninstall.bat", "uninstaller.bat", "uninstall.msi"
    };

    QDir installDir(installPath);
    for (const QString &uninstallExe : uninstallExes) {
        QString path = installDir.filePath(uninstallExe);
        if (QFile::exists(path))
            return path;
    }

    QString parentDir = QFileInfo(installPath).absolutePath();
    for (const QString &uninstallExe : uninstallExes) {
        QString path = QDir(parentDir).filePath(uninstallExe);
        if (QFile::exists(path))
            return path;
    }

    const QStringList regPaths = {
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
    };

    for (const QString &regPath : regPaths) {
        QSettings settings(regPath, QSettings::NativeFormat);
        for (const QString &subKey : settings.childGroups()) {
            settings.beginGroup(subKey);
            QString regInstallLocation = settings.value("InstallLocation").toString();
            QString uninstallString = settings.value("UninstallString").toString();
            settings.endGroup();

            if (installPath.contains(regInstallLocation) && !uninstallString.isEmpty()) {
                return uninstallString;
            }
        }
    }

    return "";
}

QString InstalledSoftware::findMainExecutable(const QString &installPath) const {
    if (installPath.isEmpty()) return "";

    QDir dir(installPath);
    if (!dir.exists()) return "";

    QStringList exeFiles = dir.entryList(QStringList() << "*.exe", QDir::Files, QDir::Time);
    if (!exeFiles.isEmpty()) {
        return dir.filePath(exeFiles.first());
    }

    return "";
}

QStringList InstalledSoftware::getAllLocalIPs() const {
    QStringList ips;
    for (const QNetworkInterface &qinterface : QNetworkInterface::allInterfaces()) {
        if (qinterface.flags().testFlag(QNetworkInterface::IsLoopBack) ||
            !qinterface.flags().testFlag(QNetworkInterface::IsUp)) {
            continue;
        }

        for (const QNetworkAddressEntry &entry : qinterface.addressEntries()) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                ips.append(entry.ip().toString());
            }
        }
    }

    return ips;
}

// ---------------------
// 以下为保存和删除 RemoteApp 软件信息到注册表的实现
// 注册表路径使用：
// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Terminal Server\TSAppAllowList\Applications
// ---------------------

bool InstalledSoftware::saveSoftwareInfo(const SoftwareInfo &info) {
    HKEY hKey;
    LPCWSTR subKey = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Terminal Server\\TSAppAllowList\\Applications";

    // 打开或创建主键
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, subKey, 0, NULL, 0,
                        KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        qDebug() << "[Error] Failed to open registry key for saving software info.";
        return false;
    }

    HKEY hAppKey;
    std::wstring appKeyName = info.name.toStdWString();
    if (RegCreateKeyExW(hKey, appKeyName.c_str(), 0, NULL, 0,
                        KEY_WRITE, NULL, &hAppKey, NULL) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        qDebug() << "[Error] Failed to create registry entry for" << info.name;
        return false;
    }

    // 写入软件名称
    std::wstring nameW = info.name.toStdWString();
    if (RegSetValueExW(hAppKey, L"Name", 0, REG_SZ,
                       reinterpret_cast<const BYTE*>(nameW.c_str()),
                       (nameW.size() + 1) * sizeof(wchar_t)) != ERROR_SUCCESS) {
        qDebug() << "[Error] Failed to set registry value for Name.";
        RegCloseKey(hAppKey);
        RegCloseKey(hKey);
        return false;
    }

    // 写入主程序路径
    std::wstring pathW = info.mainExePath.toStdWString();
    if (RegSetValueExW(hAppKey, L"Path", 0, REG_SZ,
                       reinterpret_cast<const BYTE*>(pathW.c_str()),
                       (pathW.size() + 1) * sizeof(wchar_t)) != ERROR_SUCCESS) {
        qDebug() << "[Error] Failed to set registry value for Path.";
        RegCloseKey(hAppKey);
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hAppKey);
    RegCloseKey(hKey);
    return true;
}

bool InstalledSoftware::deleteSoftwareInfo(const SoftwareInfo &info) {
    HKEY hKey;
    LPCWSTR subKey = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Terminal Server\\TSAppAllowList\\Applications";

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, subKey, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        qDebug() << "[Error] Failed to open registry key for deleting software info.";
        return false;
    }

    std::wstring appKeyName = info.name.toStdWString();
    LONG status = RegDeleteTreeW(hKey, appKeyName.c_str());
    RegCloseKey(hKey);

    if (status == ERROR_SUCCESS) {
        return true;
    } else {
        qDebug() << "[Error] Failed to delete registry entry for" << info.name;
        return false;
    }
}

#endif
