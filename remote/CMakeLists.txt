cmake_minimum_required(VERSION 3.16)

project(remote VERSION 0.1 LANGUAGES CXX)

# ------------------------------------------------------------------------------
# 全局宏定义：必须在 find_package(Qt6 ...) 之前定义，以作用于 moc/autogen
# ------------------------------------------------------------------------------
add_compile_definitions(
    WIN32_LEAN_AND_MEAN  # 排除 rarely-used Windows headers（含 winsock.h）
    NOMINMAX             # 禁止定义 min/max 宏，避免与 std::min/std::max 冲突
)

# ------------------------------------------------------------------------------
# 基本设置
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_NO_SHOW_OLD_POLICY_WARNINGS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ------------------------------------------------------------------------------
# 查找 Qt 组件
# ------------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Quick
    Sql
    Multimedia
)

# ------------------------------------------------------------------------------
# 可执行文件 & QML 模块
# ------------------------------------------------------------------------------
qt_add_executable(appremote
    App.rc
)

qt_add_qml_module(appremote
    URI remote
    VERSION 1.0
    QML_FILES Page/Login.qml
    SOURCES globalproperties.h globalproperties.cpp
    QML_FILES Page/RCCpage/Mainwindow.qml
    SOURCES Code/Users/account.h Code/Users/account.cpp
    QML_FILES Page/Register.qml
    SOURCES Login.cpp
    QML_FILES Page/Dialog/SuecessDialog.qml
    QML_FILES Page/Main.qml
    QML_FILES Model/DeviceInformationModel.qml
    QML_FILES Model/DeviceAppModel.qml
    SOURCES Code/DataBase/database.h Code/DataBase/database.cpp
    QML_FILES Page/Dialog/SystemErrorDialog.qml
    SOURCES Code/AppData/devicedate.h
    SOURCES imageresource.h imageresource.cpp
    QML_FILES Page/RCCpage/UserDevices.qml
    QML_FILES Page/RCCpage/RemoteSetting.qml
    QML_FILES Page/RCCpage/DirectConnect.qml
    QML_FILES Page/RCRpage/RCRMainwindow.qml
    QML_FILES Page/RCCpage/RemoteDraw.qml
    RESOURCES resource.qrc
    QML_FILES Page/RCRpage/PersonalSetting.qml
    QML_FILES Page/RCRpage/WinApplicationPreview.qml
    QML_FILES Page/RCCpage/RemoteAppList.qml
    SOURCES Code/LogUntils/AppLog.cpp Code/LogUntils/AppLog.h
    SOURCES Code/PID/AdaptivePIDController.h
    SOURCES Code/PID/AdaptivePIDController.cpp
    SOURCES Code/RDP/remotecontrol.h Code/RDP/remotecontrol.cpp
    SOURCES Code/AppData/installedSoftware.cpp
    SOURCES Code/AppData/installedSoftware.h
    SOURCES Code/TCP/tcpconnection.h Code/TCP/tcpconnection.cpp
    SOURCES Code/RDP/remotecontrolthread.h Code/RDP/remotecontrolthread.cpp
    SOURCES Code/AppData/devicedata.cpp
    SOURCES Code/TCP/tcpservertest.h Code/TCP/tcpservertest.cpp
    SOURCES Code/RDP/remoteimageprovider.h Code/RDP/remoteimageprovider.cpp
    SOURCES Code/TCP/tcpserverthread.h Code/TCP/tcpserverthread.cpp
    SOURCES Code/Users/userdevice.h Code/Users/userdevice.cpp
    QML_FILES Page/RCCpage/FileDialogs.qml
    SOURCES Code/AppData/deviceinfomanager.h Code/AppData/deviceinfomanager.cpp
)

# Windows 下隐藏控制台窗口
set_target_properties(appremote PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# 链接 Qt 库
target_link_libraries(appremote
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Quick
        Qt6::Sql
        Qt6::Multimedia
)

# ------------------------------------------------------------------------------
# 平台特定依赖
# ------------------------------------------------------------------------------
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Compiling for Linux")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FREERDP REQUIRED freerdp2)
    pkg_check_modules(WINPR   REQUIRED winpr2)

    find_library(CZMQ_LIB czmq REQUIRED)

    target_include_directories(appremote PRIVATE
        ${FREERDP_INCLUDE_DIRS}
        ${WINPR_INCLUDE_DIRS}
    )

    target_link_libraries(appremote PRIVATE
        ${FREERDP_LIBRARIES}
        ${WINPR_LIBRARIES}
        ${CZMQ_LIB}
    )

    target_compile_definitions(appremote PRIVATE LINUX)
endif()

# 如果是 Windows 系统
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Compiling for Windows")

    # 查找系统库 shlwapi
    find_library(SHLWAPI_LIBRARY shlwapi)
    if(SHLWAPI_LIBRARY)
        message(STATUS "Found shlwapi.lib: ${SHLWAPI_LIBRARY}")
        target_link_libraries(appremote PRIVATE ${SHLWAPI_LIBRARY})
    else()
        message(WARNING "shlwapi.lib not found.")
    endif()
    set(CZMQ_LIB_DIR "C:/Users/86184/Desktop/vcpkg/packages/czmq_x64-windows/lib")
    set(CZMQ_DLL_DIR "C:/Users/86184/Desktop/vcpkg/packages/czmq_x64-windows/bin")
    set(CZMQ_INCLUDE_DIR "C:/Users/86184/Desktop/vcpkg/packages/czmq_x64-windows/include")
    set(ZeroMQ_LIB_DIR "C:/Users/86184/Desktop/vcpkg/packages/zeromq_x64-windows/lib")
    set(ZeroMQ_DLL_DIR "C:/Users/86184/Desktop/vcpkg/packages/zeromq_x64-windows/bin")
    set(ZeroMQ_INCLUDE_DIR "C:/Users/86184/Desktop/vcpkg/packages/zeromq_x64-windows/include")
    include_directories(${CZMQ_INCLUDE_DIR})
    include_directories(${ZeroMQ_INCLUDE_DIR})
    link_directories(${CZMQ_DLL_DIR} ${ZeroMQ_DLL_DIR})

    # 链接库
    target_link_libraries(appremote PRIVATE
        shell32.lib
        ole32.lib
        ${CZMQ_LIB_DIR}/czmq.lib
        ${ZeroMQ_LIB_DIR}/libzmq-mt-4_3_5.lib
    )
endif()

# ------------------------------------------------------------------------------
# 安装规则
# ------------------------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS appremote
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
