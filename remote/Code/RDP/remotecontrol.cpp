#include "remotecontrol.h"
#include <QDebug>
#include <cstring>

#ifdef LINUX
#include <freerdp/log.h>
#define TAG "RemoteControl"

// Custom log callback function
void my_log_callback(wLog* log, int level, const char* msg) {
    if (level >= WLOG_TRACE) {
        qDebug() << msg;
    }
}

RemoteControl::RemoteControl(QObject *parent)
    : QObject(parent), _instance(nullptr), _context(nullptr), _settings(nullptr)
{
}

RemoteControl::~RemoteControl()
{
    disconnect();
}

bool RemoteControl::initialize()
{
    qDebug() << "[RemoteControl] 初始化 FreeRDP...";

    _instance = freerdp_new();
    if (!_instance) {
        qDebug() << "创建 FreeRDP 实例失败";
        return false;
    }

    // 设置自定义上下文大小
    _instance->ContextSize = sizeof(RemoteControlContext);
    if (!freerdp_context_new(_instance)) {
        qDebug() << "创建上下文失败";
        freerdp_free(_instance);
        _instance = nullptr;
        return false;
    }
    _context = reinterpret_cast<RemoteControlContext*>(_instance->context);
    _context->remoteControl = this; // 存储当前实例的指针

    // 初始化设置
    _settings = _instance->settings;
    if (!_settings) {
        qDebug() << "[RemoteControl] 错误：无法初始化 FreeRDP 设置";
        freerdp_context_free(_instance);
        freerdp_free(_instance);
        _context = nullptr;
        _instance = nullptr;
        return false;
    }

    // Configure settings
    _settings->RemoteFxCodec = FALSE;
    _settings->NSCodec = TRUE;
    _settings->DesktopWidth = 1280;
    _settings->DesktopHeight = 720;
    _settings->SurfaceCommandsEnabled = TRUE;
    _settings->ColorDepth = 8;
    if (!gdi_init(_instance, PIXEL_FORMAT_BGRA32)) {
        qDebug() << "[ERROR] GDI 初始化失败";
        return false;
    }

    // 注册图形回调
    _instance->update->BeginPaint = [](rdpContext* context) -> BOOL {
        qDebug() << "BeginPaint: 准备接收图形更新";
        return TRUE;
    };

    _instance->update->EndPaint = [](rdpContext* context) -> BOOL {
        qDebug() << "EndPaint: 图形更新已接收";
        RemoteControlContext* myContext = reinterpret_cast<RemoteControlContext*>(context);
        RemoteControl* control = myContext->remoteControl;
        if (control) {
            QMetaObject::invokeMethod(control, "requestRedraw");
        }
        return TRUE;
    };
    // 注册其他必要回调（如键盘、鼠标）
    _instance->input->KeyboardEvent = handle_keyboard_event;
    _instance->input->MouseEvent = handle_mouse_event;
    qDebug() << "FreeRDP initialized successfully";
    return true;
}

bool RemoteControl::connect(const QString& hostname, const QString& username, const QString& password)
{
    qDebug() << "[RemoteControl] Starting FreeRDP connection";
    if (!_instance) {
        qDebug() << "[RemoteControl] Error: FreeRDP instance not initialized";
        return false;
    }

    if (_settings) {
        _settings->ServerHostname = strdup(hostname.toUtf8().constData());
        _settings->Username = strdup(username.toUtf8().constData());
        _settings->Password = strdup(password.toUtf8().constData());
        _settings->ServerPort = 3389;
        _settings->IgnoreCertificate = TRUE;
        _settings->RdpSecurity = TRUE;
        _settings->TlsSecurity = FALSE;
        _settings->NlaSecurity = FALSE;
        // _settings->SetBoolValue(FREERDP_MOUSE_MOTION, TRUE);
    } else {
        qDebug() << "[RemoteControl] Error: FreeRDP settings not initialized";
        return false;
    }

    if (!freerdp_connect(_instance)) {
        qDebug() << "[RemoteControl] Error: Unable to connect to RDP server";
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }

    if (freerdp_channels_attach(_instance) != 0) {
        qDebug() << "[RemoteControl] Error: Failed to attach channels";
        freerdp_disconnect(_instance);
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }

    qDebug() << "[RemoteControl] Info: Connected to RDP server: " << hostname.toUtf8().constData();
    return true;
}

void RemoteControl::disconnect()
{
    if (!_instance) {
        qDebug() << "[RemoteControl] Info: No connection to disconnect";
        return;
    }

    qDebug() << "Disconnecting from RDP server";
    freerdp_disconnect(_instance);
    freerdp_context_free(_instance);
    freerdp_free(_instance);

    // Free allocated memory
    if (_settings) {
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
    }

    _instance = nullptr;
    _context = nullptr;
    _settings = nullptr;
}

void RemoteControl::runEventLoop()
{
    qDebug() << "Running event loop";
    while (_instance && !freerdp_shall_disconnect(_instance)) {
        int ret = freerdp_check_event_handles(_instance->context);
        if (ret < 0) {
            qDebug() << "[RemoteControl] Error: Event loop failed";
            break;
        }
    }
    qDebug() << "[RemoteControl] Info: Event loop exited";
}

void RemoteControl::requestRedraw() {
    QMetaObject::invokeMethod(this, [this]() {
        if (!_settings || !_context || !_context->context.gdi) {
            qDebug() << "[ERROR] Invalid graphics context";
            return;
        }

        BYTE* buffer = _context->context.gdi->primary_buffer;
        int width = _settings->DesktopWidth;
        int height = _settings->DesktopHeight;

        // 检查缓冲区有效性
        if (!buffer || width <= 0 || height <= 0) {
            qDebug() << "[ERROR] Invalid buffer or dimensions";
            return;
        }

        // 创建 QImage 并发射信号
        _remoteImage = QImage(buffer, width, height, QImage::Format_RGB32);
        qDebug() << "Emitting imageUpdated, size:" << _remoteImage.size();
        emit imageUpdated(_remoteImage);
    });
}

QImage RemoteControl::currentImage() const {
    return _remoteImage; // 返回当前存储的远程图像
}

// RemoteControl.cpp
BOOL RemoteControl::handle_keyboard_event(rdpInput* input, UINT16 flags, UINT16 code) {
    // 处理键盘事件
    qDebug() << "键盘事件: flags=" << flags << ", code=" << code;
    return TRUE;
}

BOOL RemoteControl::handle_mouse_event(rdpInput* input, UINT16 flags, UINT16 x, UINT16 y) {
    qDebug() << "鼠标事件处理函数调用，flags=" << flags << ", x=" << x << ", y=" << y;
    // 根据需要处理鼠标事件
    return freerdp_input_send_mouse_event(input, flags, x, y);
}
void RemoteControl::sendMouseEvent(int x, int y, int buttonFlags, int releaseFlags) {
    if (!_instance || !_instance->input) {
        qDebug() << "FreeRDP connection is not active";
        return;
    }
    if (_instance && _instance->input) {
        UINT16 remoteX = static_cast<UINT16>(x);
        UINT16 remoteY = static_cast<UINT16>(y);

        if (freerdp_input_send_mouse_event(_instance->input, buttonFlags, remoteX, remoteY)) {
            if (releaseFlags != 0) {
                freerdp_input_send_mouse_event(_instance->input, releaseFlags, remoteX, remoteY);
            }
        }
    }
}
#endif
