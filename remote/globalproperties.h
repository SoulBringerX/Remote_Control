#ifndef GLOBALPROPERTIES_H
#define GLOBALPROPERTIES_H

#include <QObject>

class GlobalProperties : public QObject
{
    Q_OBJECT // 必须添加这个宏
    Q_PROPERTY(bool preview READ getPreview CONSTANT)

public:
    explicit GlobalProperties(QObject *parent = nullptr);

    // 类的公有取值方法“GlobalProperties.preview”
    bool getPreview() const;

    // 单例模式的实现
    static GlobalProperties* getInstance();
};

#endif // GLOBALPROPERTIES_H
