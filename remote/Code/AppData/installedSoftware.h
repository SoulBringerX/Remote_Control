#ifndef INSTALLEDSOFTWARE_H
#define INSTALLEDSOFTWARE_H

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <windows.h>

class InstalledSoftware : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList softwareList READ softwareList NOTIFY softwareListChanged)

public:
    explicit InstalledSoftware(QObject *parent = nullptr);
    QVariantList softwareList() const;

signals:
    void softwareListChanged();

private:
    QVariantList softwareList_;
    void refreshSoftwareList();
};

#endif // INSTALLEDSOFTWARE_H
