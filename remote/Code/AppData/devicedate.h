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

// 设定这个数据传输包
#pragma pack(push, 1)
struct RD_Packet {
    char RD_IP[16];
    char RD_Username[33];
    char RD_Password[33];
    unsigned char RD_Type;
    char RD_ImageBit[1024];
    char RD_APP_Name[33];
    char RD_Command_Name[65];

    RD_Packet() {
        memset(this, 0, sizeof(RD_Packet));
    }
};
#pragma pack(pop)

#endif // DEVICEDATE_H
