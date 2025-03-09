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
    logger.print("RemoteRDP","RDP连接初始化中》》》》》");

    _instance = freerdp_new();
    if (!_instance) {
        logger.print("RemoteRDP","创建RDP实例失败");
        return false;
    }

    // 设置自定义上下文大小
    _instance->ContextSize = sizeof(RemoteControlContext);
    if (!freerdp_context_new(_instance)) {
        logger.print("RemoteRDP","创建RDP的context失败");
        freerdp_free(_instance);
        logger.print("RemoteRDP","释放RDP实例");
        _instance = nullptr;
        return false;
    }
    _context = reinterpret_cast<RemoteControlContext*>(_instance->context);
    _context->remoteControl = this; // 存储当前实例的指针

    // 初始化设置
    _settings = _instance->settings;
    if (!_settings) {
        logger.print("RemoteRDP","无法初始化RDP的setting配置，setting配置不存在");
        freerdp_context_free(_instance);
        logger.print("RemoteRDP","释放RDP实例context");
        freerdp_free(_instance);
        logger.print("RemoteRDP","释放RDP实例");
        _context = nullptr;
        _instance = nullptr;
        return false;
    }

    // Configure settings
    _settings->RemoteFxCodec = FALSE;
    _settings->NSCodec = TRUE;
    _settings->DesktopWidth = 1920;
    _settings->DesktopHeight = 1080;
    _settings->SurfaceCommandsEnabled = TRUE;
    _settings->ColorDepth = 32;
    if (!gdi_init(_instance, PIXEL_FORMAT_BGRA32)) {
        logger.print("RemoteRDP","GDI配置无法初始化");
        return false;
    }

    // 注册图形回调
    _instance->update->BeginPaint = [](rdpContext* context) -> BOOL {
        logger.print("RemoteRDP","BeginPaint: 准备接收图形更新");
        return TRUE;
    };

    _instance->update->EndPaint = [](rdpContext* context) -> BOOL {
        logger.print("RemoteRDP","EndPaint: 图形更新已接收");
        RemoteControlContext* myContext = reinterpret_cast<RemoteControlContext*>(context);
        RemoteControl* control = myContext->remoteControl;
        if (control) {
            QMetaObject::invokeMethod(control, "requestRedraw");
        }
        return TRUE;
    };
    // // 注册其他必要回调（如键盘、鼠标）暂时貌似不需要使用回调
    // _instance->input->KeyboardEvent = handle_keyboard_event;
    // _instance->input->MouseEvent = handle_mouse_event;
    logger.print("RemoteRDP","RDP初始化成功");
    return true;
}

bool RemoteControl::connect(const QString& hostname, const QString& username, const QString& password)
{
    logger.print("RemoteRDP","RDP连接开始》》》》》》》");
    if (!_instance) {
        logger.print("RemoteRDP","RDP实例并未初始化或RDP实例初始化信息失效");
        return false;
    }

    if (_settings) {
        _settings->ServerHostname = strdup(hostname.toUtf8().constData());
        _settings->Username = strdup(username.toUtf8().constData());
        _settings->Password = strdup(password.toUtf8().constData());
        _settings->ServerPort = 3389;
        _settings->IgnoreCertificate = TRUE;
        _settings->RemoteFxCodec = FALSE;
        _settings->NSCodec = TRUE;
        _settings->RdpSecurity = TRUE;
        _settings->TlsSecurity = FALSE;
        _settings->NlaSecurity = FALSE;
        _settings->SurfaceCommandsEnabled = TRUE;
        _settings->AudioPlayback = TRUE;   // 启用音频播放
        _settings->AudioCapture = TRUE;    // 启用音频捕获
        _settings->RemoteConsoleAudio = TRUE;
        freerdp_settings_set_bool(_settings, FreeRDP_TlsSecLevel, TRUE);
        // _settings->SetBoolValue(FREERDP_MOUSE_MOTION, TRUE);
    } else {
        logger.print("RemoteRDP","RDP的settings并未初始化并配置");
        return false;
    }
    if (!freerdp_connect(_instance)) {
        logger.print("RemoteRDP","RDP的settings并未初始化并配置");
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }
    if (freerdp_channels_attach(_instance) != 0) {
        logger.print("RemoteRDP","通道附加失败，错误代码: " + QString::number(freerdp_get_last_error(_instance->context)));
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }
    logger.print("RemoteRDP","RDP连接设备成功，IP： " + hostname.toUtf8());
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
    int retryCount = 0;
        constexpr int MAX_RETRY = 3;

        while (_instance && retryCount < MAX_RETRY) {
            int ret = freerdp_check_event_handles(_instance->context);

            if (ret == 0) { // 正常处理
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
QPointF RemoteControl::convertToRemoteCoordinates(qreal localX, qreal localY, const QSize& widgetSize) {
    if (widgetSize.isValid() && _settings &&
        widgetSize.width() > 0 && widgetSize.height() > 0) {
        // 计算缩放比例
        qreal scaleX = static_cast<qreal>(_settings->DesktopWidth) / widgetSize.width();
        qreal scaleY = static_cast<qreal>(_settings->DesktopHeight) / widgetSize.height();

        // 转换坐标
        qreal remoteX = localX * scaleX;
        qreal remoteY = localY * scaleY;

        // 边界检查
        remoteX = qBound(0.0, remoteX, static_cast<qreal>(_settings->DesktopWidth - 1));
        remoteY = qBound(0.0, remoteY, static_cast<qreal>(_settings->DesktopHeight - 1));

        return QPointF(remoteX, remoteY);
    }
    return QPointF(-1, -1); // 无效情况返回 (-1, -1)
}

BOOL RemoteControl::handle_mouse_event(rdpInput* input, UINT16 flags, UINT16 x, UINT16 y) {
    qDebug() << "鼠标事件处理函数调用，flags=" << flags << ", x=" << x << ", y=" << y;
    // 根据需要处理鼠标事件
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
#endif
