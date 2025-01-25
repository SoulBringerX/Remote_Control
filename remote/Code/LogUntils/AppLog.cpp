#include "AppLog.h"
namespace LoggerNamespace {

void logger::print(QString page, QString message) {
    // 实现日志打印逻辑
    qDebug() << "[" << page << "]" << message;
}

// 初始化全局实例
logger globalLogger;

} // namespace LoggerNamespace

// 初始化全局引用
LoggerNamespace::logger& logger = LoggerNamespace::globalLogger;