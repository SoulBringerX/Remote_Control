#include "remotecontrol.h"
#include <QDebug>
#include <cstring>

#ifdef LINUX

#define TAG "RemoteControl"

RemoteControl::RemoteControl(QObject *parent)
    : QObject(parent), _instance(nullptr), _context(nullptr), _settings(nullptr) {
}

RemoteControl::~RemoteControl() {
    disconnect();
}

bool RemoteControl::initialize() {
    qDebug() << "[RemoteControl] Initializing FreeRDP...";

    _instance = freerdp_new();
    qDebug() << "[RemoteControl] Initializing FreeRDP_freerdp_new";
    qDebug() << "[RemoteControl] Initializing FreeRDP "<<_instance;
    if (!_instance) {
        qDebug() << "Failed to create FreeRDP instance";
        return false;
    }
    // 注册连接回调
    _instance->PostConnect = [](freerdp* instance) {
        qDebug() << "Connected!";
        return TRUE;
    };

    // 创建上下文
    if (!freerdp_context_new(_instance)) {
        qDebug() << "Failed to create context";
        freerdp_free(_instance);
        _instance = nullptr;
        return false;
    }
    // 初始化设置
   _settings = _instance->settings;
   if (!_settings) {
       qDebug() << "[RemoteControl] ERROR: Failed to initialize FreeRDP settings";
       freerdp_free(_instance);
       _context = nullptr;
       _instance = nullptr;
       return false;
   }

    qDebug() << "FreeRDP initialized successfully";
    return true;
}

bool RemoteControl::connect(const QString& hostname, const QString& username, const QString& password) {
    qDebug() << "[RemoteControl] DEBUG: FreeRDP start link";
    if (!_instance) {
        qDebug() << "[RemoteControl] ERROR: FreeRDP instance is not initialized";
        return false;
    }

    qDebug() << "[RemoteControl] DEBUG: FreeRDP start link_setting";
    // 设置连接参数
    if(_settings)
    {
        _settings->ServerHostname = strdup(hostname.toUtf8().constData());
        _settings->Username = strdup(username.toUtf8().constData());
        _settings->Password = strdup(password.toUtf8().constData());
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting_user finished";
        _settings->ServerPort = 3389;                     // RDP 默认端口
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting_port finished";
        _settings->IgnoreCertificate = TRUE;        // 忽略证书错误（仅用于测试）
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting_ig finished";
        _settings->TlsSecurity = TRUE;       // 启用 TLS
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting_tls finished";
        _settings->NlaSecurity = TRUE;       // 启用 NLA
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting_nla finished";
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting finished";
    }
    else
    {
        qDebug() << "[RemoteControl] ERROR: FreeRDP setting is not initialized";
        return false;
    }
    // 连接到远程桌面
    if (freerdp_connect(_instance) != 0) {
        qDebug() << "[RemoteControl] ERROR: Failed to connect to RDP server";
        return false;
    }

    qDebug() << "[RemoteControl] INFO: Connected to RDP server: " << hostname.toUtf8().constData();
    return true;
}

void RemoteControl::disconnect() {
    if (!_instance) {
        qDebug() << "[RemoteControl] INFO: Nothing to disconnect";
        return;
    }

    qDebug() << "Disconnecting from RDP server";
    freerdp_disconnect(_instance);
    freerdp_free(_instance);
    _instance = nullptr;
    _context = nullptr;
    _settings = nullptr;
}

void RemoteControl::runEventLoop() {
    if (!_instance) {
        qDebug() << "[RemoteControl] ERROR: FreeRDP instance is not initialized";
        return;
    }

    // 主事件循环
    while (true) {
        int ret = freerdp_check_event_handles(_context);
        if (ret < 0) {
            qDebug() << "[RemoteControl] ERROR: Failed to check event handles";
            break;
        }
        if (freerdp_shall_disconnect(_instance)) {
            qDebug() << "[RemoteControl] INFO: Disconnecting from RDP server";
            break;
        }
    }
}

#endif
