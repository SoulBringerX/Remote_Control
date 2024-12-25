#include "imageresource.h"
#include <QImageReader>
#include <QFile>
#include <QBuffer>
#include <QDebug>
#include <QFileInfo>

ImageResource::ImageResource() : _imageName(""), _accountImage() {}

// 加载图片并解析为QImage对象
bool ImageResource::loadImage(const QString &filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "File path is empty!";
        return false;
    }

    _accountImage.load(filePath);  // 使用QImage加载图片
    if (_accountImage.isNull()) {
        qWarning() << "Failed to load image from:" << filePath;
        return false;
    }

    _imageName = QFileInfo(filePath).fileName();  // 获取图片文件名
    return true;
}

// 将图片转换为二进制数据
QByteArray ImageResource::readImageAsBinary(const QString &filePath)
{
    QByteArray byteArray;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        byteArray = file.readAll();  // 读取文件内容为二进制数据
        file.close();
    } else {
        qWarning() << "Failed to open file for reading:" << filePath;
    }

    return byteArray;
}

// 获取当前加载的图片
QImage ImageResource::getImage() const
{
    return _accountImage;
}
