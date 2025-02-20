#include "remotecontrol.h"
#include <QDebug>
#include <cstring>
#include <freerdp/log.h>  // 确保包含这个头文件

#ifdef LINUX

#define TAG "RemoteControl"

// 自定义日志回调函数声明
void my_log_callback(wLog* log, int level, const char* msg) {
    // 根据级别输出日志
    if (level >= WLOG_TRACE) {  // 控制输出级别
        qDebug() << msg;
    }
}

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

    // 获取根日志实例并设置回调
    wLog* log = WLog_GetRoot();
    WLog_SetLogLevel(log, WLOG_TRACE);          // 设置日志级别为 TRACE

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
    if(_settings) {
        _settings->ServerHostname = strdup(hostname.toUtf8().constData());
        _settings->Username = strdup(username.toUtf8().constData());
        _settings->Password = strdup(password.toUtf8().constData());
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting_user finished";
        _settings->ServerPort = 3389; // RDP 默认端口
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting_port finished";
        _settings->IgnoreCertificate = TRUE;
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting_ig finished";
        _settings->RdpSecurity = TRUE;  // 启用 RDP 加密
        _settings->TlsSecurity = FALSE; // 禁用 TLS 加密
        _settings->NlaSecurity = FALSE; // 禁用 NLA 加密
        qDebug() << "[RemoteControl] DEBUG: FreeRDP link_setting finished";
    }
    else {
        qDebug() << "[RemoteControl] ERROR: FreeRDP setting is not initialized";
        return false;
    }

    // 打印连接信息
    if (_settings) {
        qDebug() << "[RemoteControl] ServerHostname: " << _settings->ServerHostname;
        qDebug() << "[RemoteControl] Username: " << _settings->Username;
        qDebug() << "[RemoteControl] ServerPort: " << _settings->ServerPort;
        qDebug() << "[RemoteControl] IgnoreCertificate: " << (_settings->IgnoreCertificate ? "TRUE" : "FALSE");
        qDebug() << "[RemoteControl] TlsSecurity: " << (_settings->TlsSecurity ? "TRUE" : "FALSE");
        qDebug() << "[RemoteControl] NlaSecurity: " << (_settings->NlaSecurity ? "TRUE" : "FALSE");
    }

    // 获取根日志实例并设置回调
    wLog* log = WLog_GetRoot();
    WLog_SetLogLevel(log, WLOG_TRACE);          // 设置日志级别为 TRACE

    // 连接到远程桌面
    if(_instance) {
        if (freerdp_connect(_instance) != 0) {
            qDebug() << "[RemoteControl] ERROR: Failed to connect to RDP server";
            if (_instance->context) {
                qDebug() << "[RemoteControl] Connection context error: " << _instance->context->argc;
            }
            return false;
        }
        else {
            qDebug() << "[RemoteControl] INFO: Successfully connected to RDP server";
        }
    }
    else {
        qDebug() << "Failed to create FreeRDP instance";
        return false;
    }

    // 初始化通道
    if (freerdp_channels_attach(_instance) != 0) {
        qDebug() << "[RemoteControl] ERROR: Failed to attach channels";
        freerdp_disconnect(_instance);
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
