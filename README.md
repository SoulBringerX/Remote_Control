# RemoteControl

#### 介绍
本仓库是基于RDP协议的远程桌面控制软件，给予FreeRDP源码操作，旨在为国产操作系统提供非适配国产操作系统软件的完整体验。


#### Widows安装教程

1.  xxxx
2.  xxxx
3.  xxxx

#### Linux安装教程
1.  xxxx

#### 使用说明

1.  见使用文档（目前还在完善代码阶段）

#### 软件更新日志
1. 2024.9.12 启动该项目并撰写项目方案。
2. 2024.9.19 完成登录界面和大部分的主页(涉及到用户连接远程电脑设备的链接)界面。
3. 2024.12.18 汲取原有的安卓移植鸿蒙项目经验，将原有的固定点布局换用百分比，已修改登录页面。
4. 2024.12.25 修改弹窗界面，采用无边框设计，其弹窗内容文本泛用性增强。
5. 2024.12.30 降低主页界面的耦合性，采用Loader加载对应的子页面。并大部分界面换用无边框设计。
6. 2025.1.2 新增托盘图标，并添加最小化到托盘和从托盘恢复的功能（针对Windows的功能，需要进一步优化）。
7. 2025.1.12 新增直接连接远程电脑的页面，并添加了连接远程电脑的按钮。
8. 2025.1.25 归纳分类代码/界面，加入全局logger日志输出。
9. 2025.2.10 成功连接数据库，登陆注册功能测试完毕，Linux端部分成功链接FreeRDP库并完成初步的远程连接代码，待测试，Win端新增预览本地安装的应用界面，还需通过注册表获取本地应用列表（包括应用图标、应用的exe文件路径（包含卸载的路径））
10. 2025.2.22 win端和linux端均已经在cmake中链接上czmq库，该库将用于TCP大数据传输。FreeRDP的相关库函数在linux端中初始化FreeRDP数据成功，但是使用freerdp_connect函数莫名直接程序闪退（已经单开一个线程），被连接的windows设备（用本人的surface go1代）已经显示处于远程连接会话中了，问题待进一步排查。
