#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QObject>
#include <QDebug>

namespace LoggerNamespace {

class logger : public QObject
{
    Q_OBJECT
public:
    logger(QObject *parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE void print(QString page, QString message);
};

// 全局实例
extern logger globalLogger;

} // namespace LoggerNamespace

// 全局引用
extern LoggerNamespace::logger& logger;

#endif // LOGGER_H