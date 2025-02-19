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
    qDebug() << "[RemoteControl] INFO: Initializing FreeRDP instance...";

    // 创建FreeRDP实例
    freerdp* instance = freerdp_new();
    if (!instance) {
        qDebug() << "[RemoteControl] ERROR: freerdp_new() failed";
        return false;
    }

    // 设置客户端模式（必须在context_new之前）
    instance->settings->ServerMode = FALSE;

    // 初始化上下文（注意返回的是BOOL）
    if (!freerdp_context_new(instance)) {
        qDebug() << "[RemoteControl] ERROR: freerdp_context_new() failed";
        freerdp_free(instance);
        return false;
    }

    _instance = instance;  // 保存实例指针
    qDebug() << "[RemoteControl] INFO: FreeRDP context created successfully.";

    // 获取上下文指针
    _context = instance->context;
    if (!_context) {
        qDebug() << "[RemoteControl] ERROR: Context is null";
        freerdp_context_free(instance);
        freerdp_free(instance);
        return false;
    }

    // 获取设置参数
    _settings = _context->settings;
    if (!_settings) {
        qDebug() << "[RemoteControl] ERROR: Failed to get settings";
        freerdp_context_free(instance);
        freerdp_free(instance);
        return false;
    }

    qDebug() << "[RemoteControl] INFO: FreeRDP initialized successfully";
    return true;
}

bool RemoteControl::connect(const QString& hostname, const QString& username, const QString& password) {
    if (!_instance) {
        qDebug() << "[RemoteControl] ERROR: FreeRDP instance is not initialized";
        return false;
    }

    // 设置连接参数
    _settings->ServerHostname = strdup(hostname.toUtf8().constData());
    _settings->Username = strdup(username.toUtf8().constData());
    _settings->Password = strdup(password.toUtf8().constData());
    _settings->ServerPort = 3389;                     // RDP 默认端口
    _settings->IgnoreCertificate = TRUE;        // 忽略证书错误（仅用于测试）
    _settings->AuthenticationOnly = FALSE;      // 完整连接模式

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
