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

    // 桌面路径（暂时未使用）
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

            // 检查是否同时具有卸载程序和具体执行程序
            bool hasUninstallProgram = !uninstallString.isEmpty();
            bool hasExecutable = false;

            // 检查安装路径是否有效，并搜索可执行文件 (*.exe)
            if (!installLocation.isEmpty()) {
                QDir dir(installLocation);
                if (dir.exists()) {
                    QStringList exeFiles = dir.entryList(QStringList() << "*.exe", QDir::Files);
                    if (!exeFiles.isEmpty()) {
                        hasExecutable = true;
                    }
                }
            }

            // 同时满足条件的软件才会被添加到列表中
            if (!displayName.isEmpty() && hasUninstallProgram && hasExecutable) {
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
    for (const auto& item : softwareList_) {
        qDebug() << "Name:" << item.toMap()["name"].toString();
        qDebug() << "Version:" << item.toMap()["version"].toString();
        qDebug() << "Install Date:" << item.toMap()["installDate"].toString();
        qDebug() << "Uninstall Path:" << item.toMap()["uninstallPath"].toString();
        qDebug() << "Install Location:" << item.toMap()["installLocation"].toString();
        qDebug() << "Publisher:" << item.toMap()["publisher"].toString();
        qDebug() << "----------------------------------------";
    }

    // 发出信号，通知 QML 数据已更新
    emit softwareListChanged();
}
QByteArray InstalledSoftware::getIconBinaryData(const QString &exePath) const
{
    QByteArray iconData;

    // 使用 QPixmap 从 exe 文件中提取图标
    QPixmap pixmap;
    QIcon icon(exePath);
    if (icon.isNull()) {
        return iconData;
    }

    pixmap = icon.pixmap(32, 32); // 设置图标大小，例如 32x32

    if (pixmap.isNull()) {
        return iconData;
    }

    QBuffer buffer(&iconData);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG"); // 可以根据需要选择格式，如 "PNG" 或 "BMP"

    return iconData;
}
#endif
