// 在 remotecontrol.h 中：
// 将 sendMouseEvent 声明为返回 BOOL 的成员函数
// static BOOL handle_mouse_event(rdpInput* input, UINT16 flags, UINT16 x, UINT16 y);
// BOOL sendMouseEvent(int x, int y, int buttonFlags, int releaseFlags);

#include "remotecontrol.h"
#include <QDebug>
#include <cstring>
#include <QTimer>
#include <QThread>

#ifdef LINUX
#include <freerdp/log.h>
#define TAG "RemoteControl"

// 构造函数与析构函数
RemoteControl::RemoteControl(QObject *parent)
    : QObject(parent), _instance(nullptr), _context(nullptr), _settings(nullptr), _remoteImage()
{
}

RemoteControl::~RemoteControl()
{
    disconnect();
}

// 初始化RDP连接及回调注册
bool RemoteControl::initialize()
{
    logger.print("RemoteRDP", "RDP连接初始化中》》》》》");

    _instance = freerdp_new();
    if (!_instance) {
        logger.print("RemoteRDP", "创建RDP实例失败");
        return false;
    }

    // 设置自定义上下文大小并创建context
    _instance->ContextSize = sizeof(RemoteControlContext);
    if (!freerdp_context_new(_instance)) {
        logger.print("RemoteRDP", "创建RDP的context失败");
        freerdp_free(_instance);
        logger.print("RemoteRDP", "释放RDP实例");
        _instance = nullptr;
        return false;
    }
    _context = reinterpret_cast<RemoteControlContext*>(_instance->context);
    _context->remoteControl = this; // 存储当前实例的指针

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

    // 输出初始设置
    qDebug() << "[RemoteControl::initialize] Initial _settings->DesktopWidth:" << _settings->DesktopWidth;
    qDebug() << "[RemoteControl::initialize] Initial _settings->DesktopHeight:" << _settings->DesktopHeight;
    _settings->RemoteFxCodec = FALSE;
    _settings->NSCodec = TRUE;
    _settings->DesktopWidth = 1920;
    _settings->DesktopHeight = 1080;
    _settings->SurfaceCommandsEnabled = TRUE;
    _settings->ColorDepth = 8;
    if (!gdi_init(_instance, PIXEL_FORMAT_BGRA32)) {
        logger.print("RemoteRDP", "GDI配置无法初始化");
        return false;
    }

    // 注册图形更新回调
    _instance->update->BeginPaint = [](rdpContext* context) -> BOOL {
        //logger.print("RemoteRDP", "BeginPaint: 准备接收图形更新");
        return TRUE;
    };

    _instance->update->EndPaint = [](rdpContext* context) -> BOOL {
        //logger.print("RemoteRDP", "EndPaint: 图形更新已接收");
        RemoteControlContext* myContext = reinterpret_cast<RemoteControlContext*>(context);
        RemoteControl* control = myContext->remoteControl;
        if (control) {
            QMetaObject::invokeMethod(control, "requestRedraw");
        }
        return TRUE;
    };

    // 注册输入回调：使用静态函数
    _instance->input->KeyboardEvent = RemoteControl::handle_keyboard_event;
    _instance->input->MouseEvent    = RemoteControl::handle_mouse_event;
    qDebug() << "[RemoteControl::initialize] Mouse event callback registered:"
             << (_instance->input->MouseEvent == RemoteControl::handle_mouse_event);
    logger.print("RemoteRDP", "RDP初始化成功");
    return true;
}

// 建立连接（错误处理略）
bool RemoteControl::connect(const QString& hostname, const QString& username, const QString& password)
{
    logger.print("RemoteRDP", "RDP连接开始》》》》》》》");
    if (!_instance) {
        logger.print("RemoteRDP", "RDP实例并未初始化或RDP实例初始化信息失效");
        return false;
    }

    if (_settings) {
        _settings->ServerHostname = strdup(hostname.toUtf8().constData());
        _settings->Username       = strdup(username.toUtf8().constData());
        _settings->Password       = strdup(password.toUtf8().constData());
        _settings->ServerPort     = 3389;
        _settings->IgnoreCertificate = TRUE;
        _settings->RemoteFxCodec  = FALSE;
        _settings->NSCodec        = TRUE;
        _settings->RdpSecurity    = TRUE;
        _settings->TlsSecurity    = FALSE;
        _settings->NlaSecurity    = FALSE;
        _settings->SurfaceCommandsEnabled = TRUE;
        freerdp_settings_set_bool(_settings, FreeRDP_TlsSecLevel, TRUE);
        _instance->input->MouseEvent    = RemoteControl::handle_mouse_event;
        _instance->input->KeyboardEvent = RemoteControl::handle_keyboard_event;
        qDebug() << "[RemoteControl::connect] 鼠标事件回调注册状态:" << (_instance->input->MouseEvent != nullptr);
        qDebug() << "[RemoteControl::connect] 键盘事件回调注册状态:" << (_instance->input->KeyboardEvent != nullptr);
    } else {
        logger.print("RemoteRDP", "RDP的settings并未初始化并配置");
        return false;
    }
    if (!freerdp_connect(_instance)) {
        logger.print("RemoteRDP", "连接失败");
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }
    if (freerdp_channels_attach(_instance) != 0) {
        logger.print("RemoteRDP", "通道附加失败，错误代码:" + QString::number(freerdp_get_last_error(_instance->context)));
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
        return false;
    }
    qDebug() << "[RemoteControl::connect] After connect, _settings->DesktopWidth:" << _settings->DesktopWidth;
    qDebug() << "[RemoteControl::connect] After connect, _settings->DesktopHeight:" << _settings->DesktopHeight;

    logger.print("RemoteRDP", "RDP连接设备成功，IP：" + hostname.toUtf8());
    return true;
}

// 断开连接及资源释放
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

    if (_settings) {
        free(_settings->ServerHostname);
        free(_settings->Username);
        free(_settings->Password);
    }

    _instance = nullptr;
    _context  = nullptr;
    _settings = nullptr;
    _remoteImage = QImage(); // 重置图像
}

// 事件循环处理及重连逻辑
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

        if (ret < 0) { // 错误处理：重连
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

// 图像重绘请求，通过信号通知界面更新
void RemoteControl::requestRedraw() {
    QMetaObject::invokeMethod(this, [this]() {
        if (!_settings || !_context || !_context->context.gdi) {
            qDebug() << "[ERROR] Invalid graphics context";
            return;
        }

        BYTE* buffer = _context->context.gdi->primary_buffer;
        int width  = _settings->DesktopWidth;
        int height = _settings->DesktopHeight;

        if (!buffer || width <= 0 || height <= 0) {
            qDebug() << "[ERROR] Invalid buffer or dimensions";
            return;
        }

        _remoteImage = QImage(buffer, width, height, QImage::Format_RGB32);
        //qDebug() << "Emitting imageUpdated, size:" << _remoteImage.size();
        emit imageUpdated(_remoteImage);
        //qDebug() << "[RemoteControl::requestRedraw] _remoteImage size after update:"<< _remoteImage.width() << "x" << _remoteImage.height();
    });
}

// 返回当前存储的远程图像
QImage RemoteControl::currentImage() const {
    return _remoteImage;
}


// 静态鼠标事件处理回调，调用 sendMouseEvent 并返回其结果
BOOL RemoteControl::handle_mouse_event(rdpInput* input, UINT16 flags, UINT16 x, UINT16 y)
{
    rdpContext* context = input->context;
    RemoteControlContext* myContext = reinterpret_cast<RemoteControlContext*>(context);
    if (myContext && myContext->remoteControl) {
        RemoteControl* rc = myContext->remoteControl;
        qDebug() << "[handle_mouse_event] 调用成功，flags:" << flags
                 << ", 原始坐标:" << x << y;
        // 根据 flags 判断按下或释放：这里只处理单击的简单逻辑
        if (flags & PTR_FLAGS_DOWN) {
            return rc->sendMouseEvent(x, y, 1, 0);
        } else {
            return rc->sendMouseEvent(x, y, 0, 1);
        }
    }
    qDebug() << "[handle_mouse_event] 无法获取 remoteControl 实例";
    return FALSE;
}

// 静态键盘事件处理回调
BOOL RemoteControl::handle_keyboard_event(rdpInput* input, UINT16 flags, UINT16 code)
{
    rdpContext* context = input->context;
    RemoteControlContext* myContext = reinterpret_cast<RemoteControlContext*>(context);
    if (myContext && myContext->remoteControl) {
        qDebug() << "[handle_keyboard_event] 调用成功，flags:" << flags << ", code:" << code;
    } else {
        qDebug() << "[handle_keyboard_event] 无法获取 remoteControl 实例";
    }
    return freerdp_input_send_keyboard_event(input, flags, code);
}

// 坐标转换，将本地坐标转换为远程桌面的坐标
QPointF RemoteControl::convertToRemoteCoordinates(qreal localX, qreal localY) {
    qDebug() << "[convertToRemoteCoordinates] 本地X坐标=" << localX << ", 本地Y坐标=" << localY;
    if (_remoteImage.isNull() || _remoteImage.width() <= 0 || _remoteImage.height() <= 0 ||
        !_settings || _settings->DesktopWidth <= 0 || _settings->DesktopHeight <= 0) {
        qDebug() << "[convertToRemoteCoordinates] 无效的图像或设置";
        return QPointF(-1, -1);
    }

    qDebug() << "[convertToRemoteCoordinates] _remoteImage 尺寸:" << _remoteImage.width()
             << "x" << _remoteImage.height();
    qDebug() << "[convertToRemoteCoordinates] _settings 桌面尺寸:" << _settings->DesktopWidth
             << "x" << _settings->DesktopHeight;

    qreal scaleX = static_cast<qreal>(_settings->DesktopWidth) / _remoteImage.width();
    qreal scaleY = static_cast<qreal>(_settings->DesktopHeight) / _remoteImage.height();
    QPointF remotePoint(localX * scaleX, localY * scaleY);

    // 确保坐标在有效范围内
    remotePoint.setX(qBound(0.0, remotePoint.x(), static_cast<qreal>(_settings->DesktopWidth - 1)));
    remotePoint.setY(qBound(0.0, remotePoint.y(), static_cast<qreal>(_settings->DesktopHeight - 1)));

    qDebug() << "[convertToRemoteCoordinates] X轴缩放比例=" << scaleX
             << ", Y轴缩放比例=" << scaleY
             << ", 远程X坐标=" << remotePoint.x()
             << ", 远程Y坐标=" << remotePoint.y();
    return remotePoint;
}

BOOL RemoteControl::sendMouseEvent(int x, int y, int buttonFlags, int releaseFlags) {
    QPointF remoteCoords = convertToRemoteCoordinates(x, y);
    if (remoteCoords.x() < 0 || remoteCoords.y() < 0) {
        qDebug() << "[sendMouseEvent] 坐标转换失败";
        return FALSE;
    }

    UINT16 remoteX = static_cast<UINT16>(remoteCoords.x());
    UINT16 remoteY = static_cast<UINT16>(remoteCoords.y());
    qDebug() << "[sendMouseEvent] 远程坐标 (转换后): (" << remoteX << "," << remoteY << ")";

    BOOL result = TRUE;

    if (buttonFlags != 0) {
        UINT16 flagsToSend = PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON1;
        qDebug() << "[sendMouseEvent] 发送鼠标按下事件, flags: 0x" << Qt::hex << flagsToSend;
        if (!freerdp_input_send_mouse_event(_instance->input, flagsToSend, remoteX, remoteY)) {
            qDebug() << "[sendMouseEvent] 鼠标按下事件发送失败";
            result = FALSE;
        } else {
            qDebug() << "[sendMouseEvent] 鼠标按下事件发送成功";
        }
    }

    if (releaseFlags != 0) {
        UINT16 flagsToSend = PTR_FLAGS_BUTTON1; // 只使用按钮标志，不加 PTR_FLAGS_UP
        qDebug() << "[sendMouseEvent] 发送鼠标释放事件, flags: 0x" << Qt::hex << flagsToSend;
        if (!freerdp_input_send_mouse_event(_instance->input, flagsToSend, remoteX, remoteY)) {
            qDebug() << "[sendMouseEvent] 鼠标释放事件发送失败";
            result = FALSE;
        } else {
            qDebug() << "[sendMouseEvent] 鼠标释放事件发送成功";
        }
    }
    return result;
}

#endif
