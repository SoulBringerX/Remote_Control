#ifndef DEVICEINFOMANAGER_H
#define DEVICEINFOMANAGER_H

#include <QObject>
#include <QVariantMap>

class DeviceInfoManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap deviceInfo READ getDeviceInfo NOTIFY deviceInfoChanged)

public:
    static DeviceInfoManager* getInstance();

    QVariantMap getDeviceInfo() const;

public slots:
    void updateDeviceInfo(QVariantMap info);

signals:
    void deviceInfoChanged();

private:
    explicit DeviceInfoManager(QObject *parent = nullptr);
    QVariantMap m_deviceInfo;
};

#endif // DEVICEINFOMANAGER_H
