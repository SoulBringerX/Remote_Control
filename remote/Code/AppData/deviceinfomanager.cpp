#include "deviceinfomanager.h"

// 静态方法，返回单例实例
DeviceInfoManager* DeviceInfoManager::getInstance() {
    static DeviceInfoManager instance;
    return &instance;
}

// 构造函数（私有）
DeviceInfoManager::DeviceInfoManager(QObject *parent) : QObject(parent) {}

// 获取设备信息
QVariantMap DeviceInfoManager::getDeviceInfo() const {
    return m_deviceInfo;
}

// 更新设备信息
void DeviceInfoManager::updateDeviceInfo(QVariantMap info) {
    m_deviceInfo = info;
    emit deviceInfoChanged();
}
