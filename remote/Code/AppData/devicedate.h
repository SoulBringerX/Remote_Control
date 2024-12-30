#ifndef DEVICEDATE_H
#define DEVICEDATE_H


#include <QString>
// 此处是设备相关信息用于主页最右侧的设备信息展示
class Devicedata
{
    QString device_id;
    QString device_name;
    QString CPU;
    int RAM;
    QString system_version;
    QString* disk;

    void init();
    void getDeviceData();
    void clean();
};


#endif // DEVICEDATE_H
