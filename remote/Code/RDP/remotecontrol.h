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

// WinPR 头文件（FreeRDP 的依赖库）
#include <winpr/wtypes.h>             // WinPR 类型定义
#include <winpr/synch.h>              // 同步工具
#include <winpr/thread.h>             // 线程支持
#include <winpr/collections.h>        // 集合工具
#include <winpr/stream.h>             // 流处理
#include <winpr/crypto.h>             // 加密工具
#include <QString>
#include <QObject>

class RemoteControl : public QObject
{
        Q_OBJECT
    public:
        explicit RemoteControl(QObject *parent = nullptr);
        ~RemoteControl();
        // 初始化 FreeRDP 实例
        Q_INVOKABLE bool initialize();

        // 连接到远程桌面
        Q_INVOKABLE bool connect(const QString& hostname, const QString& username, const QString& password);

        // 断开连接
        Q_INVOKABLE void disconnect();

        // 处理事件循环
        Q_INVOKABLE void runEventLoop();

    private:
        freerdp* _instance;       // FreeRDP 实例
        rdpContext* _context;     // FreeRDP 上下文
        rdpSettings* _settings;   // FreeRDP 设置
};
#endif
#endif // REMOTECONTROL_H
