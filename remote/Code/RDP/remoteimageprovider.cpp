#ifdef LINUX
#include "remoteimageprovider.h"

RemoteImageProvider::RemoteImageProvider(RemoteControl* remoteControl)
    : QQuickImageProvider(QQuickImageProvider::Image),
      m_remoteControl(remoteControl) {}

QImage RemoteImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize) {
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);

    if (m_remoteControl) {
        QImage image = m_remoteControl->currentImage();
        if (size) *size = image.size();
        //qDebug() << "Providing image, size:" << image.size(); // 调试输出
        return image;
    }
    return QImage();
}
#endif
