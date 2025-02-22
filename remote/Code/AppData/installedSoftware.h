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
#include <QStandardPaths>
#include <shlwapi.h>
#include <windows.h>


class InstalledSoftware : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList softwareList READ softwareList NOTIFY softwareListChanged)

public:
    explicit InstalledSoftware(QObject *parent = nullptr);
    // 新增：获取图标二进制数据
    QByteArray getIconBinaryData(const QString &exePath) const;
    QVariantList softwareList() const;

signals:
    void softwareListChanged();

private:
    QVariantList softwareList_;
    void refreshSoftwareList();
};

#endif // INSTALLEDSOFTWARE_H
#endif
