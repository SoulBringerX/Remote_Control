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

// 定义操作指令类型枚举
enum class OperationCommandType : unsigned char {
    TransmitAppAlias = 0x01,     // 传输应用别名
    TransmitAppIconData = 0x02,  // 传输应用图标数据
    TransmitAppCommand = 0x03,   // 传输应用程序名
    TransmitConnectTest = 0x04,  // TCP传输结果测试
    TransmitEnd = 0x00           // 传输结束
};


// 设定这个数据传输包
#pragma pack(push, 1)  // 开启1字节对齐
struct RD_Packet {
    char RD_IP[16];                     // 远端设备IP
    char RD_Username[33];               // 远端设备的账户名
    char RD_Password[33];               // 远端设备的密码
    OperationCommandType RD_Type;       // 操作指令类型
    char RD_ImageBit[1024];             // 应用图标数据
    char RD_APP_Name[33];               // 应用别名
    char RD_Command_Name[65];           // 应用程序名
    char RD_MainExePath[512];           // 主程序路径
    char RD_UninstallExePath[512];      // 卸载程序路径

    RD_Packet() {
        memset(this, 0, sizeof(RD_Packet));  // 初始化时将所有字段清零
    }
};
#pragma pack(pop)  // 关闭1字节对齐

#endif // DEVICEDATE_H
