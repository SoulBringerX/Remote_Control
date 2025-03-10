#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#ifdef LINUX
#include <freerdp/freerdp.h>
#include <freerdp/api.h>
#include <freerdp/settings.h>
#include <freerdp/peer.h>
#include <freerdp/client/channels.h>
#include <freerdp/gdi/gdi.h>
#include <freerdp/gdi/gfx.h>
#include <freerdp/codec/color.h>
#include <freerdp/codec/rfx.h>
#include <freerdp/codec/nsc.h>
#include <freerdp/crypto/crypto.h>
#include <freerdp/log.h>
#include <freerdp/input.h>

#include <winpr/wtypes.h>
#include <winpr/synch.h>
#include <winpr/thread.h>
#include <winpr/collections.h>
#include <winpr/stream.h>
#include <winpr/crypto.h>

#include <QString>
#include <QObject>
#include <QImage>
#include <QTimer>
#include <QThread>
#include <QKeyEvent>
#include <QDebug>

#include "../LogUntils/AppLog.h"

class RemoteControl;

struct RemoteControlContext {
    rdpContext context;
    RemoteControl* remoteControl;
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

    // 静态回调函数
    Q_INVOKABLE static BOOL handle_keyboard_event(rdpInput* input, UINT16 flags, UINT16 code);
    Q_INVOKABLE static BOOL handle_mouse_event(rdpInput* input, UINT16 flags, UINT16 x, UINT16 y);

    Q_INVOKABLE QPointF convertToRemoteCoordinates(qreal localX, qreal localY, const QSize& widgetSize);

    // **重载 Qt 按键转换函数，适用于 QML**
    Q_INVOKABLE UINT16 convertQtKeyToRdpKey(int qtKey, const QString& text, int nativeScanCode);
    UINT16 convertQtKeyToRdpKey(int qtKey, const QString& text, int nativeScanCode, bool* extended);

public slots:
    Q_INVOKABLE void sendMouseEvent(int x, int y, int buttonFlags, int releaseFlags);
    Q_INVOKABLE BOOL sendKeyboardEvent(bool down, UINT16 keycode, bool extended);
    Q_INVOKABLE BOOL sendUnicodeKeyboardEvent(bool down, UINT16 code, bool extended);
signals:
    void imageUpdated(const QImage& image);
    void disconnected();
private:
    freerdp* _instance;
    RemoteControlContext* _context;
    rdpSettings* _settings;
    QImage _remoteImage;
};

#endif // LINUX
#endif // REMOTECONTROL_H
