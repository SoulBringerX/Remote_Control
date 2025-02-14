#ifdef WIN32

#include "installedsoftware.h"
#include <QVariantList>
#include <QDebug>
#include <QSettings>
#include <shlwapi.h>

InstalledSoftware::InstalledSoftware(QObject *parent)
    : QObject(parent) {
    refreshSoftwareList();
}

QVariantList InstalledSoftware::softwareList() const {
    return softwareList_;
}

void InstalledSoftware::refreshSoftwareList() {
    softwareList_.clear();

    // 获取桌面路径
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    // 检查并遍历注册表中的卸载信息路径
    QStringList regPaths = {
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"
    };

    for (const QString& regPath : regPaths) {
        QSettings settings(regPath, QSettings::NativeFormat);

        // 遍历各个子项
        for (const QString& regItem : settings.childGroups()) {
            settings.beginGroup(regItem);

            // 获取软件信息
            QString displayName = settings.value("DisplayName").toString();
            QString displayVersion = settings.value("DisplayVersion").toString();
            QString installLocation = settings.value("InstallLocation").toString();
            QString publisher = settings.value("Publisher").toString();
            QString uninstallString = settings.value("UninstallString").toString();
            QString installDate = settings.value("InstallDate").toString();

            // 检查软件在桌面上是否有快捷方式
            QStringList shortcuts = QDir(desktopPath).entryList(QStringList() << "*.lnk" << "*.url", QDir::Files);
            bool hasShortcut = false;

            foreach (const QString& shortcut, shortcuts) {
                // 忽略文件扩展名，检查软件名是否包含在快捷方式名称中
                QString shortcutName = QFileInfo(shortcut).baseName(); // 获取快捷方式名称
                if (shortcutName.contains(displayName, Qt::CaseInsensitive)) {
                    hasShortcut = true;
                    break;
                }
            }

            // 仅当软件名不为空且拥有桌面快捷方式时添加到软件列表中
            if (!displayName.isEmpty() && hasShortcut) {
                QVariantMap softwareInfo;
                softwareInfo.insert("name", displayName);
                softwareInfo.insert("version", displayVersion);
                softwareInfo.insert("installLocation", installLocation);
                softwareInfo.insert("publisher", publisher);
                softwareInfo.insert("uninstallPath", uninstallString);
                softwareInfo.insert("installDate", installDate);

                softwareList_.append(softwareInfo);
            }

            settings.endGroup();
        }
    }

    // 打印日志，验证数据是否正确填充
    qDebug() << "Software List:";
    for (const auto &item : softwareList_) {
        qDebug() << "Name:" << item.toMap()["name"].toString();
        qDebug() << "Version:" << item.toMap()["version"].toString();
        qDebug() << "Install Date:" << item.toMap()["installDate"].toString();
    }

    // 发出信号，通知 QML 数据已更新
    emit softwareListChanged();
}
#endif
