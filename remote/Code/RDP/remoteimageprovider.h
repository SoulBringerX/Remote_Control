#ifndef REMOTEIMAGEPROVIDER_H
#define REMOTEIMAGEPROVIDER_H

#ifdef LINUX
#include <QQuickImageProvider>
#include "./remotecontrol.h"
#include "../LogUntils/AppLog.h"

class RemoteImageProvider : public QQuickImageProvider {
public:
    RemoteImageProvider(RemoteControl* remoteControl);

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

private:
    RemoteControl* m_remoteControl;
};

#endif
#endif // REMOTEIMAGEPROVIDER_H
