#include "remotecontrol.h"
#include <QDebug>
#define TAG "RemoteControl"
RemoteControl::RemoteControl(QObject *parent)
    : QObject(parent), _instance(nullptr), _context(nullptr), _settings(nullptr) {
}

RemoteControl::~RemoteControl() {
    disconnect();
}

bool RemoteControl::initialize() {
    // 创建 FreeRDP 实例
    _instance = freerdp_new();
    if (!_instance) {
        WLog_ERR(TAG, "Failed to create FreeRDP instance");
        return false;
    }

    // 获取上下文和设置
    _context = _instance->context;
    _settings = _context->settings;

    // 加载通道插件
    if (!freerdp_channels_load_plugin(_context->channels, _settings, "cliprdr", nullptr)) {
        WLog_ERR(TAG, "Failed to load cliprdr channel plugin");
        return false;
    }

    if (!freerdp_channels_load_plugin(_context->channels, _settings, "rdpsnd", nullptr)) {
        WLog_ERR(TAG, "Failed to load rdpsnd channel plugin");
        return false;
    }

    WLog_INFO(TAG, "FreeRDP instance initialized successfully");
    return true;
}

bool RemoteControl::connect(const QString& hostname, const QString& username, const QString& password) {
    if (!_instance) {
        WLog_ERR(TAG, "FreeRDP instance is not initialized");
        return false;
    }

    // 设置连接参数
    _settings->ServerHostname = hostname.toUtf8().constData();
    _settings->Username = username.toUtf8().constData();
    _settings->Password = password.toUtf8().constData();
    _settings->ServerPort = 3389;                     // RDP 默认端口
    _settings->IgnoreCertificate = TRUE;        // 忽略证书错误（仅用于测试）
    _settings->AuthenticationOnly = FALSE;      // 完整连接模式

    // 连接到远程桌面
    if (freerdp_connect(_instance) != 0) {
        WLog_ERR(TAG, "Failed to connect to RDP server");
        return false;
    }

    WLog_INFO(TAG, "Connected to RDP server: %s", hostname.toUtf8().constData());
    return true;
}

void RemoteControl::disconnect() {
    if (_instance) {
        freerdp_disconnect(_instance);
        freerdp_free(_instance);
        _instance = nullptr;
        _context = nullptr;
        _settings = nullptr;
    }
}

void RemoteControl::runEventLoop() {
    if (!_instance) {
        WLog_ERR(TAG, "FreeRDP instance is not initialized");
        return;
    }

    // 主事件循环
    while (true) {
        int ret = freerdp_check_event_handles(_context);
        if (ret < 0) {
            WLog_ERR(TAG, "Failed to check event handles");
            break;
        }
        if (freerdp_shall_disconnect(_instance)) {
            WLog_INFO(TAG, "Disconnecting from RDP server");
            break;
        }
    }
}
