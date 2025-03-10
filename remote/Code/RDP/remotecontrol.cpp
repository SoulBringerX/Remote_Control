#include "remotecontrol.h"
#include <QDebug>
#include <cstring>

#ifdef LINUX
#include <freerdp/log.h>
#define TAG "RemoteControl"

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
    logger.print("RemoteRDP", "RDP连接初始化中》》》》》");

    _instance = freerdp_new();
    if (!_instance) {
        logger.print("RemoteRDP", "创建RDP实例失败");
        return false;
    }

    // 设置自定义上下文大小，并创建 context
    _instance->ContextSize = sizeof(RemoteControlContext);
    if (!freerdp_context_new(_instance)) {
        logger.print("RemoteRDP", "创建RDP的context失败");
        freerdp_free(_instance);
        logger.print("RemoteRDP", "释放RDP实例");
        _instance = nullptr;
        return false;
    }
    _context = reinterpret_cast<RemoteControlContext*>(_instance->context);
    _context->remoteControl = this;

    // 初始化设置
    _settings = _instance->settings;
    if (!_settings) {
        logger.print("RemoteRDP", "无法初始化RDP的setting配置，setting配置不存在");
        freerdp_context_free(_instance);
        logger.print("RemoteRDP", "释放RDP实例context");
        freerdp_free(_instance);
        logger.print("RemoteRDP", "释放RDP实例");
        _context = nullptr;
        _instance = nullptr;
        return false;
    }

    // 配置 RDP 设置
    _settings->RemoteFxCodec = FALSE;
    _settings->NSCodec = TRUE;
    _settings->DesktopWidth = 1280;
    _settings->DesktopHeight = 720;
    _settings->SurfaceCommandsEnabled = TRUE;
    _settings->ColorDepth = 16;
    if (!gdi_init(_instance, PIXEL_FORMAT_BGRA32)) {
        logger.print("RemoteRDP", "GDI配置无法初始化");
        return false;
    }

    // 注册图形回调
    _instance->update->BeginPaint = [](rdpContext* context) -> BOOL {
        logger.print("RemoteRDP", "BeginPaint: 准备接收图形更新");
        return TRUE;
    };

    _instance->update->EndPaint = [](rdpContext* context) -> BOOL {
        logger.print("RemoteRDP", "EndPaint: 图形更新已接收");
        RemoteControlContext* myContext = reinterpret_cast<RemoteControlContext*>(context);
        RemoteControl* control = myContext->remoteControl;
        if (control) {
            QMetaObject::invokeMethod(control, "requestRedraw");
        }
        return TRUE;
    };

    // 注册键盘回调
    _instance->input->KeyboardEvent = [](rdpInput* input, UINT16 flags, UINT16 code) -> BOOL {
        qDebug() << "Received keyboard callback: flags=0x" << Qt::hex << flags
                 << "code=0x" << Qt::hex << code;
        return TRUE;
    };
    logger.print("RemoteRDP", "RDP初始化成功");
    return true;
}

bool RemoteControl::connect(const QString& hostname, const QString& username, const QString& password)
{
    logger.print("RemoteRDP", "RDP连接开始》》》》》》》");
    if (!_instance) {
        logger.print("RemoteRDP", "RDP实例并未初始化或RDP实例初始化信息失效");
        return false;
    }

    if (_settings) {
        _settings->ServerHostname = strdup(hostname.toUtf8().constData());
        _settings->Username = strdup(username.toUtf8().constData());
        _settings->Password = strdup(password.toUtf8().constData());
        _settings->ServerPort = 3389;
        _settings->IgnoreCertificate = TRUE;
        // **加载用户上次保存的配置**
        loadSettings();
        // _settings->RemoteFxCodec = FALSE;
        // _settings->NSCodec = TRUE;
        // _settings->RdpSecurity = TRUE;
        // _settings->TlsSecurity = FALSE;
        // _settings->NlaSecurity = FALSE;
        // _settings->SurfaceCommandsEnabled = TRUE;
        // _settings->AudioPlayback = TRUE;   // 启用音频播放
        // _settings->AudioCapture = TRUE;    // 启用音频捕获
        // _settings->RemoteConsoleAudio = TRUE;
    } else {
        logger.print("RemoteRDP", "RDP的settings并未初始化并配置");
        return false;
    }
    if (!freerdp_connect(_instance)) {
        logger.print("RemoteRDP", "RDP连接失败");
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }
    if (freerdp_channels_attach(_instance) != 0) {
        logger.print("RemoteRDP", "通道附加失败，错误代码: " + QString::number(freerdp_get_last_error(_instance->context)));
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }
    logger.print("RemoteRDP", "RDP连接设备成功，IP： " + hostname.toUtf8());
    return true;
}

void RemoteControl::disconnect()
{
    if (!_instance) {
        qDebug() << "[RemoteControl] Info: No active connection to disconnect";
        emit disconnected();
        return;
    }

    qDebug() << "Disconnecting from RDP server";

    // 注意确保在主线程中执行
    freerdp_disconnect(_instance);
    freerdp_context_free(_instance);
    freerdp_free(_instance);
    _instance = nullptr;
    _context = nullptr;

    if (_settings) {
        if (_settings->ServerHostname) {
            free(_settings->ServerHostname);
            _settings->ServerHostname = nullptr;
        }
        if (_settings->Username) {
            free(_settings->Username);
            _settings->Username = nullptr;
        }
        if (_settings->Password) {
            free(_settings->Password);
            _settings->Password = nullptr;
        }
        _settings = nullptr;
    }

    // 发射断开完成信号
    emit disconnected();
}

// 远程控制相关设置函数
void RemoteControl::setBpp(int index) {
    static const int bppValues[] = {8, 16, 24, 32};
    if (index >= 0 && index < 4) {
        _settings->ColorDepth = bppValues[index];
    }
}

void RemoteControl::setAudioEnabled(bool enabled) {
    _settings->AudioPlayback = enabled;
}

void RemoteControl::setMicrophoneEnabled(bool enabled) {
    _settings->AudioCapture = enabled;
}

void RemoteControl::setRemoteFxEnabled(bool enabled) {
    _settings->RemoteFxCodec = enabled;
}

void RemoteControl::setNSCodecEnabled(bool enabled) {
    _settings->NSCodec = enabled;
}

void RemoteControl::setSurfaceCommandsEnabled(bool enabled) {
    _settings->SurfaceCommandsEnabled = enabled;
}

void RemoteControl::setRemoteConsoleAudioEnabled(bool enabled) {
    _settings->RemoteConsoleAudio = enabled;
}

void RemoteControl::setDriveMappingEnabled(bool enabled) {
    _settings->DeviceRedirection = enabled;
}

void RemoteControl::setUsbRedirectionEnabled(bool enabled) {
    _settings->DeviceRedirection = enabled;
}


// 主事件循环
void RemoteControl::runEventLoop()
{
    int retryCount = 0;
    constexpr int MAX_RETRY = 3;

    while (_instance && retryCount < MAX_RETRY) {
        int ret = freerdp_check_event_handles(_instance->context);

        if (ret == 0) {
            retryCount = 0;
            continue;
        }

        // 错误处理
        if (ret < 0) {
            qDebug() << "事件循环错误，尝试重连 (" << ++retryCount << "/" << MAX_RETRY << ")";

            // 清理残留连接
            this->disconnect();

            // 延迟重连
            QThread::sleep(2);

            // 重新初始化连接
            if (!initialize() || !connect(_settings->ServerHostname,
                                          _settings->Username,
                                          _settings->Password)) {
                qDebug() << "重连失败";
                continue;
            }
        }
    }

    if (retryCount >= MAX_RETRY) {
        exit(0);
    }
}

void RemoteControl::saveSettings() {
    QSettings settings("RDP", "RemoteControlApp"); // 指定存储路径

    settings.setValue("Bpp", _settings->ColorDepth);
    settings.setValue("AudioPlayback", _settings->AudioPlayback);
    settings.setValue("AudioCapture", _settings->AudioCapture);
    settings.setValue("RemoteFxCodec", _settings->RemoteFxCodec);
    settings.setValue("NSCodec", _settings->NSCodec);
    settings.setValue("SurfaceCommandsEnabled", _settings->SurfaceCommandsEnabled);
    settings.setValue("RemoteConsoleAudio", _settings->RemoteConsoleAudio);
    settings.setValue("DriveMappingEnabled", _settings->DeviceRedirection);
    settings.setValue("UsbRedirectionEnabled", _settings->DeviceRedirection);

    qDebug() << "用户配置已保存！";
}

void RemoteControl::loadSettings() {
    QSettings settings("RDP", "RemoteControlApp");

    _settings->ColorDepth = settings.value("Bpp", 24).toInt();  // 默认 24bpp
    _settings->AudioPlayback = settings.value("AudioPlayback", true).toBool();
    _settings->AudioCapture = settings.value("AudioCapture", false).toBool();
    _settings->RemoteFxCodec = settings.value("RemoteFxCodec", false).toBool();
    _settings->NSCodec = settings.value("NSCodec", true).toBool();
    _settings->SurfaceCommandsEnabled = settings.value("SurfaceCommandsEnabled", true).toBool();
    _settings->RemoteConsoleAudio = settings.value("RemoteConsoleAudio", true).toBool();
    _settings->DeviceRedirection = settings.value("DriveMappingEnabled", false).toBool();
    _settings->DeviceRedirection = settings.value("UsbRedirectionEnabled", false).toBool();

    qDebug() << "用户配置已加载！";
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

// 远程控制中的键盘、鼠标事件

QImage RemoteControl::currentImage() const {
    return _remoteImage; // 返回当前存储的远程图像
}

// 静态键盘事件处理回调
BOOL RemoteControl::handle_keyboard_event(rdpInput* input, UINT16 flags, UINT16 code) {
    qDebug() << "键盘事件: flags=" << flags << ", code=" << code;
    return TRUE;
}

// 坐标转换：将本地坐标转换为远程桌面坐标
QPointF RemoteControl::convertToRemoteCoordinates(qreal localX, qreal localY, const QSize& widgetSize) {
    if (widgetSize.isValid() && _settings &&
        widgetSize.width() > 0 && widgetSize.height() > 0) {
        qreal scaleX = static_cast<qreal>(_settings->DesktopWidth) / widgetSize.width();
        qreal scaleY = static_cast<qreal>(_settings->DesktopHeight) / widgetSize.height();

        qreal remoteX = localX * scaleX;
        qreal remoteY = localY * scaleY;

        remoteX = qBound(0.0, remoteX, static_cast<qreal>(_settings->DesktopWidth - 1));
        remoteY = qBound(0.0, remoteY, static_cast<qreal>(_settings->DesktopHeight - 1));

        return QPointF(remoteX, remoteY);
    }
    return QPointF(-1, -1);
}

// 静态鼠标事件处理回调
BOOL RemoteControl::handle_mouse_event(rdpInput* input, UINT16 flags, UINT16 x, UINT16 y) {
    qDebug() << "鼠标事件处理函数调用，flags=" << flags << ", x=" << x << ", y=" << y;
    return freerdp_input_send_mouse_event(input, flags, x, y);
}

void RemoteControl::sendMouseEvent(int x, int y, int buttonFlags, int releaseFlags) {
    // 允许的合法标志组合
    constexpr int validButtonFlags =
        PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON1 |
        PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON2 |
        PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON3 |
        PTR_FLAGS_MOVE;

    constexpr int validReleaseFlags =
        PTR_FLAGS_BUTTON1 |
        PTR_FLAGS_BUTTON2 |
        PTR_FLAGS_BUTTON3;

    // 按下事件校验
    if (buttonFlags != 0 &&
       !(buttonFlags & validButtonFlags)) {
        qDebug() << "非法按下标志: 0x" << Qt::hex << buttonFlags;
        return;
    }

    // 释放事件校验
    if (releaseFlags != 0 &&
       !(releaseFlags & validReleaseFlags)) {
        qDebug() << "非法释放标志: 0x" << Qt::hex << releaseFlags;
        return;
    }

    // 坐标校验
    if (x < 0 || x >= _settings->DesktopWidth ||
        y < 0 || y >= _settings->DesktopHeight) {
        qDebug() << "坐标越界: (" << x << "," << y << ")";
        return;
    }

    // 发送事件
    UINT16 remoteX = static_cast<UINT16>(x);
    UINT16 remoteY = static_cast<UINT16>(y);

    if (buttonFlags != 0) {
        if (!freerdp_input_send_mouse_event(_instance->input, buttonFlags, remoteX, remoteY)) {
            qDebug() << "鼠标按下事件发送失败";
        } else {
            qDebug() << "鼠标按下事件发送成功，横坐标：" << remoteX << "，纵坐标：" << remoteY;
        }
    }

    if (releaseFlags != 0) {
        QTimer::singleShot(50, [=]() {
            if (!freerdp_input_send_mouse_event(_instance->input, releaseFlags, remoteX, remoteY)) {
                qDebug() << "鼠标释放事件发送失败";
            } else {
                qDebug() << "鼠标释放事件发送成功，横坐标：" << remoteX << "，纵坐标：" << remoteY;
            }
        });
    }
}

BOOL RemoteControl::sendKeyboardEvent(bool down, UINT16 keycode, bool extended) {
    if (!_instance || !_instance->input) {
        qDebug() << "Error: RDP input instance not initialized";
        return FALSE;
    }

    UINT16 flags = down ? KBD_FLAGS_DOWN : KBD_FLAGS_RELEASE;
    if (extended) {
        flags |= KBD_FLAGS_EXTENDED;
    }

    qDebug() << "Sending keyboard event: down=" << down
             << "keycode=0x" << Qt::hex << keycode
             << "flags=0x" << Qt::hex << flags;

    BOOL result = freerdp_input_send_keyboard_event(_instance->input, flags, keycode);
    if (!result) {
        qDebug() << "Failed to send keyboard event. Error code:"
                 << freerdp_get_last_error(_instance->context);
    } else {
        qDebug() << "Keyboard event sent successfully";
    }
    return result;
}

BOOL RemoteControl::sendUnicodeKeyboardEvent(bool down, UINT16 code, bool extended) {
    if (!_instance || !_instance->input) {
        qDebug() << "Error: RDP input instance not initialized";
        return FALSE;
    }

    UINT16 flags = down ? KBD_FLAGS_DOWN : KBD_FLAGS_RELEASE;
    if (extended) {
        flags |= KBD_FLAGS_EXTENDED;
    }

    qDebug() << "Sending Unicode event: down=" << down
             << "code=0x" << Qt::hex << code
             << "flags=0x" << Qt::hex << flags;

    BOOL result = freerdp_input_send_unicode_keyboard_event(_instance->input, flags, code);
    if (!result) {
        qDebug() << "Failed to send Unicode event. Error code:"
                 << freerdp_get_last_error(_instance->context);
    } else {
        qDebug() << "Unicode event sent successfully";
    }
    return result;
}

// **新的 `convertQtKeyToRdpKey` 适用于 QML 调用**
UINT16 RemoteControl::convertQtKeyToRdpKey(int qtKey, const QString& text, int nativeScanCode) {
    bool extended = false;
    return convertQtKeyToRdpKey(qtKey, text, nativeScanCode, &extended);
}

// **原来的 `convertQtKeyToRdpKey`，用于 C++ 内部**
UINT16 RemoteControl::convertQtKeyToRdpKey(int qtKey, const QString& text, int nativeScanCode, bool* extended) {
    if (extended)
        *extended = false;

    qDebug() << "Converting Qt key:" << qtKey << "text:" << text << "Native scan code:" << nativeScanCode;

    switch (qtKey) {
        case Qt::Key_Backspace: return RDP_SCANCODE_BACKSPACE;
        case Qt::Key_Tab: return RDP_SCANCODE_TAB;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (nativeScanCode == 0xE01C) {
                if (extended) *extended = true;
            }
            return RDP_SCANCODE_RETURN;

        case Qt::Key_Shift:
            return (nativeScanCode == 0x36) ? RDP_SCANCODE_RSHIFT : RDP_SCANCODE_LSHIFT;

        case Qt::Key_Control:
            if (nativeScanCode == 0xE01D) {
                if (extended) *extended = true;
                return RDP_SCANCODE_RCONTROL;
            }
            return RDP_SCANCODE_LCONTROL;

        case Qt::Key_Alt:
            if (nativeScanCode == 0xE038) {
                if (extended) *extended = true;
                return RDP_SCANCODE_RMENU;
            }
            return RDP_SCANCODE_LMENU;

        case Qt::Key_A: return RDP_SCANCODE_KEY_A;
        case Qt::Key_B: return RDP_SCANCODE_KEY_B;
        case Qt::Key_C: return RDP_SCANCODE_KEY_C;
        case Qt::Key_D: return RDP_SCANCODE_KEY_D;
        case Qt::Key_E: return RDP_SCANCODE_KEY_E;
        case Qt::Key_F: return RDP_SCANCODE_KEY_F;
        case Qt::Key_G: return RDP_SCANCODE_KEY_G;
        case Qt::Key_H: return RDP_SCANCODE_KEY_H;
        case Qt::Key_I: return RDP_SCANCODE_KEY_I;
        case Qt::Key_J: return RDP_SCANCODE_KEY_J;
        case Qt::Key_K: return RDP_SCANCODE_KEY_K;
        case Qt::Key_L: return RDP_SCANCODE_KEY_L;
        case Qt::Key_M: return RDP_SCANCODE_KEY_M;
        case Qt::Key_N: return RDP_SCANCODE_KEY_N;
        case Qt::Key_O: return RDP_SCANCODE_KEY_O;
        case Qt::Key_P: return RDP_SCANCODE_KEY_P;
        case Qt::Key_Q: return RDP_SCANCODE_KEY_Q;
        case Qt::Key_R: return RDP_SCANCODE_KEY_R;
        case Qt::Key_S: return RDP_SCANCODE_KEY_S;
        case Qt::Key_T: return RDP_SCANCODE_KEY_T;
        case Qt::Key_U: return RDP_SCANCODE_KEY_U;
        case Qt::Key_V: return RDP_SCANCODE_KEY_V;
        case Qt::Key_W: return RDP_SCANCODE_KEY_W;
        case Qt::Key_X: return RDP_SCANCODE_KEY_X;
        case Qt::Key_Y: return RDP_SCANCODE_KEY_Y;
        case Qt::Key_Z: return RDP_SCANCODE_KEY_Z;


        // 数字键 0-9
        case Qt::Key_0: return RDP_SCANCODE_KEY_0;
        case Qt::Key_1: return RDP_SCANCODE_KEY_1;
        case Qt::Key_2: return RDP_SCANCODE_KEY_2;
        case Qt::Key_3: return RDP_SCANCODE_KEY_3;
        case Qt::Key_4: return RDP_SCANCODE_KEY_4;
        case Qt::Key_5: return RDP_SCANCODE_KEY_5;
        case Qt::Key_6: return RDP_SCANCODE_KEY_6;
        case Qt::Key_7: return RDP_SCANCODE_KEY_7;
        case Qt::Key_8: return RDP_SCANCODE_KEY_8;
        case Qt::Key_9: return RDP_SCANCODE_KEY_9;

        // 功能键 F1-F12
        case Qt::Key_F1: return RDP_SCANCODE_F1;
        case Qt::Key_F2: return RDP_SCANCODE_F2;
        case Qt::Key_F3: return RDP_SCANCODE_F3;
        case Qt::Key_F4: return RDP_SCANCODE_F4;
        case Qt::Key_F5: return RDP_SCANCODE_F5;
        case Qt::Key_F6: return RDP_SCANCODE_F6;
        case Qt::Key_F7: return RDP_SCANCODE_F7;
        case Qt::Key_F8: return RDP_SCANCODE_F8;
        case Qt::Key_F9: return RDP_SCANCODE_F9;
        case Qt::Key_F10: return RDP_SCANCODE_F10;
        case Qt::Key_F11: return RDP_SCANCODE_F11;
        case Qt::Key_F12: return RDP_SCANCODE_F12;

        // 箭头键
        case Qt::Key_Left:
            if (extended) *extended = true;
            return RDP_SCANCODE_LEFT;
        case Qt::Key_Right:
            if (extended) *extended = true;
            return RDP_SCANCODE_RIGHT;
        case Qt::Key_Up:
            if (extended) *extended = true;
            return RDP_SCANCODE_UP;
        case Qt::Key_Down:
            if (extended) *extended = true;
            return RDP_SCANCODE_DOWN;

        // 其他符号键
        // case Qt::Key_Minus: return RDP_SCANCODE_MINUS;         // -
        // case Qt::Key_Equal: return RDP_SCANCODE_EQUALS;        // =
        // case Qt::Key_BracketLeft: return RDP_SCANCODE_LBRACKET; // [
        // case Qt::Key_BracketRight: return RDP_SCANCODE_RBRACKET; // ]
        // case Qt::Key_Backslash: return RDP_SCANCODE_BACKSLASH; // \
        // case Qt::Key_Semicolon: return RDP_SCANCODE_SEMICOLON; // ;
        // case Qt::Key_Apostrophe: return RDP_SCANCODE_APOSTROPHE; // '
        // case Qt::Key_Comma: return RDP_SCANCODE_COMMA;         // ,
        // case Qt::Key_Period: return RDP_SCANCODE_PERIOD;       // .
        // case Qt::Key_Slash: return RDP_SCANCODE_SLASH;         // /
        case Qt::Key_Space: return RDP_SCANCODE_SPACE;         // Space

        default:
            qDebug() << "Unhandled key:" << qtKey;
            return 0;
    }
}

#endif
