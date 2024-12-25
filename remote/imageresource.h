#ifndef IMAGERESOURCE_H
#define IMAGERESOURCE_H

#include <QString>
#include <QFile>
#include <QByteArray>
#include <QImage>
#include <QBuffer>

class ImageResource
{
public:
    ImageResource();
    // 解析图片并加载为QImage
    bool loadImage(const QString &filePath);

    // 将图片转换为二进制数据
    QByteArray readImageAsBinary(const QString &filePath);

    // 获取当前加载的图片
    QImage getImage() const;

private:
    QString _imageName;  // 存储图片名称
    QImage _accountImage;  // 存储图片数据
};

#endif // IMAGERESOURCE_H
