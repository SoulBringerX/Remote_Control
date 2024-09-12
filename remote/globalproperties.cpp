#include "globalproperties.h"

GlobalProperties::GlobalProperties(QObject *parent)
    : QObject(parent)
{
    // 构造函数实现
}

bool GlobalProperties::getPreview() const
{
    // 返回 preview 属性的值
    return false;
}

// 单例模式的静态实例
static GlobalProperties instance;
GlobalProperties* GlobalProperties::getInstance()
{
    static GlobalProperties instance;
    return &instance;
}
