#include "remotecontrol.h"
#include <QDebug>
#include <cstring>

#ifdef LINUX

#define TAG "RemoteControl"

// 构造函数
RemoteControl::RemoteControl(QObject *parent)
    : QObject(parent), _instance(nullptr), _context(nullptr), _settings(nullptr)
{
}

// 析构函数
RemoteControl::~RemoteControl()
{
    disconnect();
}

// 初始化RDP连接
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
        freerdp_free(_instance);
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
        return TRUE;
    };

    _instance->update->EndPaint = [](rdpContext* context) -> BOOL {
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

// 发起RDP连接
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
        _settings->RdpSecurity = TRUE;
        _settings->TlsSecurity = TRUE;
        _settings->NlaSecurity = FALSE;
        _settings->EnableWindowsKey = TRUE;
        _settings->LogonErrors = TRUE;
        // _settings->HiDefRemoteApp = TRUE;
        // _settings->SupportMultitransport = TRUE;
        _settings->ChannelCount = 4;
        _settings->PerformanceFlags = 4;
        _settings->DynamicChannelCount = 4;
        _settings->MouseAttached = TRUE;
        _settings->MouseHasWheel = TRUE;

        // 测试
        // _settings->RemoteApplicationMode = TRUE;
        // // 设置远程应用程序的名称
        // _settings->RemoteApplicationName = "Microsoft Edge";
        // _settings->RemoteAppLanguageBarSupported = TRUE;
        // // 设置远程应用程序的程序路径
        // _settings->RemoteApplicationProgram = "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe";

        // 加载用户上次保存的配置
        loadSettings();
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
        logger.print("RemoteRDP", "通道附加失败，错误代码: " +
                     QString::number(freerdp_get_last_error(_instance->context)));
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }
    logger.print("RemoteRDP", "RDP连接设备成功，IP： " + hostname.toUtf8());
    return true;
}

// 断开RDP连接，释放资源
void RemoteControl::disconnect()
{
    if (!_instance) {
            qDebug() << "[RemoteControl] Info: No active connection to disconnect";
            emit disconnected();
            return;
        }

        qDebug() << "Disconnecting from RDP server";

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

        emit disconnected();
}

// 设置颜色深度（Bpp）
void RemoteControl::setBpp(int index) {
    static const int bppValues[] = {8, 16, 24, 32};
    if (index >= 0 && index < 4) {
        freerdp_settings_set_uint32(_settings, FreeRDP_ColorDepth, bppValues[index]);
    }
}

// 设置音频播放
void RemoteControl::setAudioEnabled(bool enabled) {
    freerdp_settings_set_bool(_settings, FreeRDP_AudioPlayback, enabled);
}

// 设置音频捕获
void RemoteControl::setMicrophoneEnabled(bool enabled) {
    freerdp_settings_set_bool(_settings, FreeRDP_AudioCapture, enabled);
}

// 设置 RemoteFx 编解码器
void RemoteControl::setRemoteFxEnabled(bool enabled) {
    freerdp_settings_set_bool(_settings, FreeRDP_RemoteFxCodec, enabled);
}

// 设置 NSCodec 编解码器
void RemoteControl::setNSCodecEnabled(bool enabled) {
    freerdp_settings_set_bool(_settings, FreeRDP_NSCodec, enabled);
}

// 设置图形命令更新
void RemoteControl::setSurfaceCommandsEnabled(bool enabled) {
    freerdp_settings_set_bool(_settings, FreeRDP_SurfaceCommandsEnabled, enabled);
}

// 设置远程控制台音频
void RemoteControl::setRemoteConsoleAudioEnabled(bool enabled) {
    // 同时设置两个相关参数
    freerdp_settings_set_bool(_settings, FreeRDP_RemoteConsoleAudio, enabled);
    freerdp_settings_set_bool(_settings, FreeRDP_AudioPlayback, enabled);
    freerdp_settings_set_bool(_settings, FreeRDP_RemoteConsoleAudio, enabled);
}

// 设置 USB 重定向（或设备重定向）
void RemoteControl::setUsbRedirectionEnabled(bool enabled) {
    freerdp_settings_set_bool(_settings, FreeRDP_DeviceRedirection, enabled);
}

// 设置磁盘映射（本地磁盘重定向）
void RemoteControl::setDriveMappingEnabled(bool enabled) {
    freerdp_settings_set_bool(_settings, FreeRDP_RedirectDrives, enabled);
    freerdp_settings_set_bool(_settings, FreeRDP_RedirectHomeDrive, enabled);
}

// 设置剪贴板重定向
void RemoteControl::setClipboardRedirectionEnabled(bool enabled) {
    freerdp_settings_set_bool(_settings, FreeRDP_RedirectClipboard, enabled);
}

// 事件循环，处理 RDP 消息与重连逻辑
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

        if (ret < 0) {
            qDebug() << "事件循环错误，尝试重连 (" << ++retryCount << "/" << MAX_RETRY << ")";

            this->disconnect();

            QThread::sleep(2);

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

// 保存用户配置
void RemoteControl::saveSettings()
{
    QSettings settings("./rdp_config.ini", QSettings::IniFormat);

    settings.setValue("Bpp", _settings->ColorDepth);
    settings.setValue("AudioPlayback", _settings->AudioPlayback);
    settings.setValue("AudioCapture", _settings->AudioCapture);
    settings.setValue("RemoteFxCodec", _settings->RemoteFxCodec);
    settings.setValue("NSCodec", _settings->NSCodec);
    settings.setValue("SurfaceCommandsEnabled", _settings->SurfaceCommandsEnabled);
    settings.setValue("RemoteConsoleAudio", _settings->RemoteConsoleAudio);
    settings.setValue("DriveMappingEnabled", _settings->RedirectHomeDrive);  // 保存磁盘映射
    settings.setValue("UsbRedirectionEnabled", _settings->DeviceRedirection);  // 保存 USB 重定向
    settings.setValue("ClipboardRedirectionEnabled", _settings->RedirectClipboard);  // 保存剪贴板重定向

    qDebug() << "用户配置已保存到 rdp_config.ini！";
}


// 加载用户配置
void RemoteControl::loadSettings()
{
    QSettings settings("./rdp_config.ini", QSettings::IniFormat);

    _settings->ColorDepth = settings.value("Bpp", 32).toInt();
    _settings->AudioPlayback = settings.value("AudioPlayback", true).toBool();
    _settings->AudioCapture = settings.value("AudioCapture", true).toBool();
    _settings->RemoteFxCodec = settings.value("RemoteFxCodec", true).toBool();
    _settings->NSCodec = settings.value("NSCodec", false).toBool();
    _settings->SurfaceCommandsEnabled = settings.value("SurfaceCommandsEnabled", true).toBool();
    _settings->RemoteConsoleAudio = settings.value("RemoteConsoleAudio", true).toBool();
    _settings->RedirectHomeDrive = settings.value("DriveMappingEnabled", false).toBool();
    _settings->DeviceRedirection = settings.value("UsbRedirectionEnabled", false).toBool();
    _settings->RedirectClipboard = settings.value("ClipboardRedirectionEnabled", false).toBool();

    qDebug() << "用户配置已从 rdp_config.ini 加载！";
}


// 获取颜色深度索引
int RemoteControl::getBppIndex() const {
    if (!_settings) return 3; // 默认 32 bpp
    int bpp = _settings->ColorDepth;
    if (bpp == 8) return 0;
    if (bpp == 16) return 1;
    if (bpp == 24) return 2;
    return 3;
}

// 获取是否启用音频
bool RemoteControl::getAudioEnabled() const {
    return _settings ? _settings->AudioPlayback : false;
}

// 获取是否启用麦克风
bool RemoteControl::getMicrophoneEnabled() const {
    return _settings ? _settings->AudioCapture : false;
}

// 获取是否启用 RemoteFx
bool RemoteControl::getRemoteFxEnabled() const {
    return _settings ? _settings->RemoteFxCodec : false;
}

// 获取是否启用 NSCodec
bool RemoteControl::getNSCodecEnabled() const {
    return _settings ? _settings->NSCodec : false;
}

// 获取是否启用 Surface Commands
bool RemoteControl::getSurfaceCommandsEnabled() const {
    return _settings ? _settings->SurfaceCommandsEnabled : false;
}

// 获取是否启用远程控制台音频
bool RemoteControl::getRemoteConsoleAudioEnabled() const {
    return _settings ? _settings->RemoteConsoleAudio : false;
}

// 获取是否启用磁盘映射
bool RemoteControl::getDriveMappingEnabled() const {
    return _settings ? _settings->RedirectHomeDrive : false;
}

// 获取是否启用设备重定向
bool RemoteControl::getUsbRedirectionEnabled() const {
    return _settings ? _settings->DeviceRedirection : false;
}
// 获取剪贴板重定向状态
bool RemoteControl::getClipboardRedirectionEnabled() const {
    return _settings ? _settings->RedirectClipboard : false;
}


// 根据 GDI 缓冲区生成 QImage 并发射更新信号
void RemoteControl::requestRedraw() {
    QMetaObject::invokeMethod(this, [this]() {
        if (!_settings || !_context || !_context->context.gdi) {
            qDebug() << "[ERROR] Invalid graphics context";
            return;
        }

        BYTE* buffer = _context->context.gdi->primary_buffer;
        int width = _settings->DesktopWidth;
        int height = _settings->DesktopHeight;

        if (!buffer || width <= 0 || height <= 0) {
            qDebug() << "[ERROR] Invalid buffer or dimensions";
            return;
        }

        _remoteImage = QImage(buffer, width, height, QImage::Format_RGB32);
        qDebug() << "Emitting imageUpdated, size:" << _remoteImage.size();
        emit imageUpdated(_remoteImage);
    });
}

// 返回当前图像
QImage RemoteControl::currentImage() const {
    return _remoteImage;
}


// Qt 界面中捕获到 QWheelEvent 后调用如下函数
void RemoteControl::handleWheelEvent(QWheelEvent* event, QSize widgetSize)
{
    if (!_instance || !_instance->input) {
        qDebug() << "[handleWheelEvent] Error: RDP input instance not initialized";
        return;
    }

    int delta = event->angleDelta().y();
    qDebug() << "[handleWheelEvent] Event triggered, delta:" << delta;

    if (delta == 0) return;

    UINT16 wheelFlag = PTR_FLAGS_WHEEL;
    if (delta < 0) wheelFlag |= PTR_FLAGS_WHEEL_NEGATIVE;
    UINT16 steps = (abs(delta) / 120) & 0xFF;

    QPointF remotePoint = convertToRemoteCoordinates(event->position().x(), event->position().y(), widgetSize);
    int x = static_cast<int>(remotePoint.x());
    int y = static_cast<int>(remotePoint.y());

    qDebug() << "[handleWheelEvent] Sending event to RDP: X=" << x << "Y=" << y << "Flag=" << wheelFlag << "Steps=" << steps;
    sendMouseEvent(x, y, wheelFlag | steps, 0);

    event->accept();
}


// 静态键盘事件回调函数
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

// 静态鼠标事件回调函数
BOOL RemoteControl::handle_mouse_event(rdpInput* input, UINT16 flags, UINT16 x, UINT16 y) {
    qDebug() << "鼠标事件处理函数调用，flags=" << flags << ", x=" << x << ", y=" << y;
    return freerdp_input_send_mouse_event(input, flags, x, y);
}

// 发送鼠标事件（按下与释放）
void RemoteControl::sendMouseEvent(int x, int y, int buttonFlags, int releaseFlags)
{
    // 有效按下标志：包含普通按键和滚轮相关标志
    constexpr int validButtonFlags =
        PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON1 |
        PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON2 |
        PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON3 |
        PTR_FLAGS_MOVE | PTR_FLAGS_WHEEL | PTR_FLAGS_HWHEEL;

    // 有效释放标志，仅针对按钮释放
    constexpr int validReleaseFlags =
        PTR_FLAGS_BUTTON1 |
        PTR_FLAGS_BUTTON2 |
        PTR_FLAGS_BUTTON3;

    // 如果 buttonFlags 中不包含滚轮标志，则进行合法性检查
    if ((buttonFlags & (PTR_FLAGS_WHEEL | PTR_FLAGS_HWHEEL)) == 0)
    {
        if (buttonFlags != 0 && !(buttonFlags & validButtonFlags))
        {
            qDebug() << "非法按下标志: 0x" << Qt::hex << buttonFlags;
            return;
        }
    }
    // 对释放标志进行检查
    if (releaseFlags != 0 && !(releaseFlags & validReleaseFlags))
    {
        qDebug() << "非法释放标志: 0x" << Qt::hex << releaseFlags;
        return;
    }
    // 检查坐标是否在桌面范围内
    if (x < 0 || x >= _settings->DesktopWidth ||
        y < 0 || y >= _settings->DesktopHeight)
    {
        qDebug() << "坐标越界: (" << x << "," << y << ")";
        return;
    }

    UINT16 remoteX = static_cast<UINT16>(x);
    UINT16 remoteY = static_cast<UINT16>(y);

    // 判断是否为滚轮事件，如果是则调用扩展鼠标事件函数
    if (buttonFlags & (PTR_FLAGS_WHEEL | PTR_FLAGS_HWHEEL))
    {
        if (!freerdp_input_send_extended_mouse_event(_instance->input, buttonFlags, remoteX, remoteY))
            qDebug() << "鼠标滚轮事件发送失败";
        else
            ;
    }
    else if (buttonFlags != 0)
    {
        if (!freerdp_input_send_mouse_event(_instance->input, buttonFlags, remoteX, remoteY))
            qDebug() << "鼠标按下事件发送失败";
        else
            ;
    }

    // 对于释放事件，只针对普通鼠标按钮发送释放
    if (releaseFlags != 0 && (releaseFlags & validReleaseFlags))
    {
        QTimer::singleShot(50, [=]() {
            if (!freerdp_input_send_mouse_event(_instance->input, releaseFlags, remoteX, remoteY))
                qDebug() << "鼠标释放事件发送失败";
            else
                ;
        });
    }
}

// 发送键盘事件
BOOL RemoteControl::sendKeyboardEvent(bool down, UINT16 keycode, bool extended) {
    if (!_instance || !_instance->input) {
        qDebug() << "Error: RDP input instance not initialized";
        return FALSE;
    }

    UINT16 flags = down ? KBD_FLAGS_DOWN : KBD_FLAGS_RELEASE;
    if (extended) flags |= KBD_FLAGS_EXTENDED;

    qDebug() << "Sending keyboard event: down=" << down
             << "keycode=0x" << Qt::hex << keycode
             << "flags=0x" << Qt::hex << flags;

    BOOL result = freerdp_input_send_keyboard_event(_instance->input, flags, keycode);
    if (!result)
        qDebug() << "Failed to send keyboard event. Error code:" << freerdp_get_last_error(_instance->context);
    else
        qDebug() << "Keyboard event sent successfully";
    return result;
}

// 发送 Unicode 键盘事件
BOOL RemoteControl::sendUnicodeKeyboardEvent(bool down, UINT16 code, bool extended) {
    if (!_instance || !_instance->input) {
        qDebug() << "Error: RDP input instance not initialized";
        return FALSE;
    }

    UINT16 flags = down ? KBD_FLAGS_DOWN : KBD_FLAGS_RELEASE;
    if (extended) flags |= KBD_FLAGS_EXTENDED;

    qDebug() << "Sending Unicode event: down=" << down
             << "code=0x" << Qt::hex << code
             << "flags=0x" << Qt::hex << flags;

    BOOL result = freerdp_input_send_unicode_keyboard_event(_instance->input, flags, code);
    if (!result)
        qDebug() << "Failed to send Unicode event. Error code:" << freerdp_get_last_error(_instance->context);
    else
        qDebug() << "Unicode event sent successfully";
    return result;
}

// 重载 Qt 按键转换函数（适用于 QML调用）
UINT16 RemoteControl::convertQtKeyToRdpKey(int qtKey, const QString& text, int nativeScanCode) {
    bool extended = false;
    return convertQtKeyToRdpKey(qtKey, text, nativeScanCode, &extended);
}

// 内部使用的 Qt 按键转换函数
UINT16 RemoteControl::convertQtKeyToRdpKey(int qtKey, const QString& text, int nativeScanCode, bool* extended)
{
    if (extended)
        *extended = false;

    qDebug() << "Converting Qt key:" << qtKey << "text:" << text << "Native scan code:" << nativeScanCode;

    switch (qtKey)
    {
        // 基本按键
        case Qt::Key_Escape:
            return RDP_SCANCODE_ESCAPE;
        case Qt::Key_1:
            return RDP_SCANCODE_KEY_1;
        case Qt::Key_2:
            return RDP_SCANCODE_KEY_2;
        case Qt::Key_3:
            return RDP_SCANCODE_KEY_3;
        case Qt::Key_4:
            return RDP_SCANCODE_KEY_4;
        case Qt::Key_5:
            return RDP_SCANCODE_KEY_5;
        case Qt::Key_6:
            return RDP_SCANCODE_KEY_6;
        case Qt::Key_7:
            return RDP_SCANCODE_KEY_7;
        case Qt::Key_8:
            return RDP_SCANCODE_KEY_8;
        case Qt::Key_9:
            return RDP_SCANCODE_KEY_9;
        case Qt::Key_0:
            return RDP_SCANCODE_KEY_0;
        case Qt::Key_Minus:
            return RDP_SCANCODE_OEM_MINUS;
        case Qt::Key_Equal:
            return RDP_SCANCODE_OEM_PLUS;
        case Qt::Key_Backspace:
            return RDP_SCANCODE_BACKSPACE;
        case Qt::Key_Tab:
            return RDP_SCANCODE_TAB;
        case Qt::Key_Q:
            return RDP_SCANCODE_KEY_Q;
        case Qt::Key_W:
            return RDP_SCANCODE_KEY_W;
        case Qt::Key_E:
            return RDP_SCANCODE_KEY_E;
        case Qt::Key_R:
            return RDP_SCANCODE_KEY_R;
        case Qt::Key_T:
            return RDP_SCANCODE_KEY_T;
        case Qt::Key_Y:
            return RDP_SCANCODE_KEY_Y;
        case Qt::Key_U:
            return RDP_SCANCODE_KEY_U;
        case Qt::Key_I:
            return RDP_SCANCODE_KEY_I;
        case Qt::Key_O:
            return RDP_SCANCODE_KEY_O;
        case Qt::Key_P:
            return RDP_SCANCODE_KEY_P;
        case Qt::Key_BracketLeft:
            return RDP_SCANCODE_OEM_4;
        case Qt::Key_BracketRight:
            return RDP_SCANCODE_OEM_6;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // 对于 Enter 键，根据 nativeScanCode 判断是否为扩展键（数字键盘 Enter）
            if (nativeScanCode == 0xE01C && extended)
                *extended = true;
            return RDP_SCANCODE_RETURN;
        case Qt::Key_Control:
            // 如果 nativeScanCode 表示右侧 Control 则设置 extended 标志
            if (nativeScanCode == 0xE01D && extended)
            {
                *extended = true;
                return RDP_SCANCODE_RCONTROL;
            }
            return RDP_SCANCODE_LCONTROL;
        case Qt::Key_A:
            return RDP_SCANCODE_KEY_A;
        case Qt::Key_S:
            return RDP_SCANCODE_KEY_S;
        case Qt::Key_D:
            return RDP_SCANCODE_KEY_D;
        case Qt::Key_F:
            return RDP_SCANCODE_KEY_F;
        case Qt::Key_G:
            return RDP_SCANCODE_KEY_G;
        case Qt::Key_H:
            return RDP_SCANCODE_KEY_H;
        case Qt::Key_J:
            return RDP_SCANCODE_KEY_J;
        case Qt::Key_K:
            return RDP_SCANCODE_KEY_K;
        case Qt::Key_L:
            return RDP_SCANCODE_KEY_L;
        case Qt::Key_Semicolon:
            return RDP_SCANCODE_OEM_1;
        case Qt::Key_Apostrophe:
            return RDP_SCANCODE_OEM_7;
        case Qt::Key_QuoteLeft:
            return RDP_SCANCODE_OEM_3;
        case Qt::Key_Shift:
            // 根据 nativeScanCode 判断左右 Shift
            return (nativeScanCode == 0x36) ? RDP_SCANCODE_RSHIFT : RDP_SCANCODE_LSHIFT;
        case Qt::Key_Backslash:
            return RDP_SCANCODE_OEM_5;
        case Qt::Key_Z:
            return RDP_SCANCODE_KEY_Z;
        case Qt::Key_X:
            return RDP_SCANCODE_KEY_X;
        case Qt::Key_C:
            return RDP_SCANCODE_KEY_C;
        case Qt::Key_V:
            return RDP_SCANCODE_KEY_V;
        case Qt::Key_B:
            return RDP_SCANCODE_KEY_B;
        case Qt::Key_N:
            return RDP_SCANCODE_KEY_N;
        case Qt::Key_M:
            return RDP_SCANCODE_KEY_M;
        case Qt::Key_Comma:
            return RDP_SCANCODE_OEM_COMMA;
        case Qt::Key_Period:
            return RDP_SCANCODE_OEM_PERIOD;
        case Qt::Key_Slash:
            return RDP_SCANCODE_OEM_2;
        case Qt::Key_Space:
            return RDP_SCANCODE_SPACE;

        // 功能键
        case Qt::Key_CapsLock:
            return RDP_SCANCODE_CAPSLOCK;
        case Qt::Key_F1:
            return RDP_SCANCODE_F1;
        case Qt::Key_F2:
            return RDP_SCANCODE_F2;
        case Qt::Key_F3:
            return RDP_SCANCODE_F3;
        case Qt::Key_F4:
            return RDP_SCANCODE_F4;
        case Qt::Key_F5:
            return RDP_SCANCODE_F5;
        case Qt::Key_F6:
            return RDP_SCANCODE_F6;
        case Qt::Key_F7:
            return RDP_SCANCODE_F7;
        case Qt::Key_F8:
            return RDP_SCANCODE_F8;
        case Qt::Key_F9:
            return RDP_SCANCODE_F9;
        case Qt::Key_F10:
            return RDP_SCANCODE_F10;
        case Qt::Key_F11:
            return RDP_SCANCODE_F11;
        case Qt::Key_F12:
            return RDP_SCANCODE_F12;

        // 数字键盘部分（扩展标志）
        case Qt::Key_NumLock:
            return RDP_SCANCODE_NUMLOCK_EXTENDED;
        case Qt::Key_Home:
            if (extended) *extended = true;
            return RDP_SCANCODE_HOME;
        case Qt::Key_Up:
            if (extended) *extended = true;
            return RDP_SCANCODE_UP;
        case Qt::Key_PageUp:
            if (extended) *extended = true;
            return RDP_SCANCODE_PRIOR;
        case Qt::Key_Left:
            if (extended) *extended = true;
            return RDP_SCANCODE_LEFT;
        case Qt::Key_Right:
            if (extended) *extended = true;
            return RDP_SCANCODE_RIGHT;
        case Qt::Key_End:
            if (extended) *extended = true;
            return RDP_SCANCODE_END;
        case Qt::Key_Down:
            if (extended) *extended = true;
            return RDP_SCANCODE_DOWN;
        case Qt::Key_PageDown:
            if (extended) *extended = true;
            return RDP_SCANCODE_NEXT;
        case Qt::Key_Insert:
            if (extended) *extended = true;
            return RDP_SCANCODE_INSERT;
        case Qt::Key_Delete:
            if (extended) *extended = true;
            return RDP_SCANCODE_DELETE;

        // // 右侧扩展键
        // case Qt::Key_Control:
        //     if (extended) *extended = true;
        //     return RDP_SCANCODE_RCONTROL;
        // case Qt::Key_AltGr:
        //     if (extended) *extended = true;
        //     return RDP_SCANCODE_RMENU;
        // case Qt::Key_Shift:
        //     if (extended) *extended = true;
        //     return RDP_SCANCODE_RSHIFT;

        // 多媒体按键（使用 Qt 标准键）
        case Qt::Key_VolumeMute:
            return RDP_SCANCODE_VOLUME_MUTE;
        case Qt::Key_VolumeDown:
            return RDP_SCANCODE_VOLUME_DOWN;
        case Qt::Key_VolumeUp:
            return RDP_SCANCODE_VOLUME_UP;

        // 浏览器按键
        case Qt::Key_Back:
            return RDP_SCANCODE_BROWSER_BACK;
        case Qt::Key_Forward:
            return RDP_SCANCODE_BROWSER_FORWARD;
        case Qt::Key_Refresh:
            return RDP_SCANCODE_BROWSER_REFRESH;
        case Qt::Key_Stop:
            return RDP_SCANCODE_BROWSER_STOP;
        case Qt::Key_Search:
            return RDP_SCANCODE_BROWSER_SEARCH;
        case Qt::Key_Favorites:
            return RDP_SCANCODE_BROWSER_FAVORITES;
        case Qt::Key_HomePage:
            return RDP_SCANCODE_BROWSER_HOME;

        // 启动按键（邮件、媒体等）
        case Qt::Key_LaunchMail:
            return RDP_SCANCODE_LAUNCH_MAIL;
        case Qt::Key_Launch0: // 此处用 Launch0 表示媒体选择键，可根据需要调整
            return RDP_SCANCODE_LAUNCH_MEDIA_SELECT;
        case Qt::Key_Launch1:
            return RDP_SCANCODE_LAUNCH_APP1;
        case Qt::Key_Launch2:
            return RDP_SCANCODE_LAUNCH_APP2;

        default:
            qDebug() << "Unhandled key:" << qtKey;
            return 0;
    }
}
void RemoteControl::onDisconnectRequested() {
    // 执行原来的断开连接逻辑
    disconnect();
}
#endif // LINUX
