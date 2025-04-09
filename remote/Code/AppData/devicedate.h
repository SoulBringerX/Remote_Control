#ifndef DEVICEDATE_H
#define DEVICEDATE_H

#include <QString>
#include <QFileInfo>
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
    TransmitAppAlias    = 0x01,            // 传输应用别名
    TransmitAppIconData = 0x02,         // 传输应用图标数据
    TransmitAppCommand  = 0x03,          // 传输应用程序名
    TransmitOnceEnd     = 0x04,             // TCP传输应用一次结束
    TransmitDeviceInformaiton   = 0x05,   //TCP传输设备硬件以及资源占用信息
    TransmitUninstallAppCommand = 0x06, //传输卸载软件的路径
    TramsmitAppData = 0x07,             //传输数据安装包
    TransmitEnd = 0x00                  // 传输结束
};

struct InstallPackageInfoNet {
    char filePath[512];   // 安装包完整路径
    char fileName[256];   // 安装包文件名
    qint64 fileSize;      // 安装包大小
};

// 函数：将 OperationCommandType 转换为对应的字符串
const char* operationCommandTypeToString(OperationCommandType type);
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
    char data[1024];                    // 其他的一些通用数据 （这里是这个设备信息）
    InstallPackageInfoNet installPackage;

    RD_Packet() {
        memset(this, 0, sizeof(RD_Packet));  // 初始化时将所有字段清零
    }
};
#pragma pack(pop)  // 关闭1字节对齐

// 定义数据块头部结构，采用1字节对齐
#pragma pack(push, 1)
struct ChunkHeader {
    OperationCommandType RD_Type; // 数据包类型，使用 TramsmitAppData 表示文件数据块
    char fileName[256];           // 文件名（若超过长度则截断）
    qint64 chunkSize;             // 本数据包中实际数据大小（字节数）
};
#pragma pack(pop)

// 定义设备信息结构体
struct DeviceInfo {
    char cpuModel[256];       // CPU 型号
    int cpuCores;             // CPU 核心数
    double cpuUsage;          // CPU 使用率（%）
    float cpuTemperature;     // CPU 温度（°C）
    quint64 totalMemory;      // 总内存（字节）
    quint64 usedMemory;       // 已用内存（字节）
    quint64 totalDisk;        // 总磁盘容量（字节）
    quint64 usedDisk;         // 已用磁盘容量（字节）
    char gpuModel[256];       // 显卡型号
    float gpuTemperature;     // 显卡温度（°C）
};

#endif // DEVICEDATE_H
