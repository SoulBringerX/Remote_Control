#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#ifdef LINUX
// FreeRDP 核心头文件
#include <freerdp/freerdp.h>          // FreeRDP 核心功能
#include <freerdp/api.h>              // FreeRDP API 定义
#include <freerdp/settings.h>         // FreeRDP 设置
#include <freerdp/peer.h>             // FreeRDP 对等连接
#include <freerdp/client/channels.h>  // FreeRDP 通道支持
#include <freerdp/gdi/gdi.h>          // GDI 图形渲染
#include <freerdp/gdi/gfx.h>          // GFX 图形渲染
#include <freerdp/codec/color.h>      // 颜色编码
#include <freerdp/codec/rfx.h>        // RemoteFX 编码
#include <freerdp/codec/nsc.h>        // NSCodec 编码
#include <freerdp/crypto/crypto.h>    // 加密支持
#include <freerdp/log.h>              // 日志工具
#include <freerdp/input.h>

// WinPR 头文件（FreeRDP 的依赖库）
#include <winpr/wtypes.h>             // WinPR 类型定义
#include <winpr/synch.h>              // 同步工具
#include <winpr/thread.h>             // 线程支持
#include <winpr/collections.h>        // 集合工具
#include <winpr/stream.h>             // 流处理
#include <winpr/crypto.h>             // 加密工具
#include <QString>
#include <QObject>
#include <QImage>
#include <QTimer>
#include <QThread>
#include "../LogUntils/AppLog.h"

// 前向声明 RemoteControl 类
class RemoteControl;

// 定义自定义上下文结构体
struct RemoteControlContext {
    rdpContext context;         // 必须放在第一个位置，以继承 rdpContext
    RemoteControl* remoteControl; // 指向 RemoteControl 对象的指针
};

class RemoteControl : public QObject {
    Q_OBJECT
public:
    explicit RemoteControl(QObject* parent = nullptr);
    ~RemoteControl();

    Q_INVOKABLE bool initialize();
    Q_INVOKABLE bool connect(const QString& hostname, const QString& username, const QString& password);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void runEventLoop();
    Q_INVOKABLE QImage currentImage() const;
    Q_INVOKABLE void requestRedraw();
    // 静态回调函数声明
    Q_INVOKABLE static BOOL handle_keyboard_event(rdpInput* input, UINT16 flags, UINT16 code);
    Q_INVOKABLE static BOOL handle_mouse_event(rdpInput* input, UINT16 flags, UINT16 x, UINT16 y);
    Q_INVOKABLE QPointF convertToRemoteCoordinates(qreal localX, qreal localY, const QSize& widgetSize);
public slots:
    Q_INVOKABLE void sendMouseEvent(int x, int y, int buttonFlags, int releaseFlags);
signals:
    void imageUpdated(const QImage& image);

private:
    freerdp* _instance;
    RemoteControlContext* _context;
    rdpSettings* _settings;
    QImage _remoteImage;
};

#endif
#endif // REMOTECONTROL_H
