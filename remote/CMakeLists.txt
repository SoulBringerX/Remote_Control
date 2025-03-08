cmake_minimum_required(VERSION 3.16)

project(remote VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11) # 指定 C++ 标准版本
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_NO_SHOW_OLD_POLICY_WARNINGS ON)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/86184/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_package(Qt6 COMPONENTS Sql REQUIRED)
find_package(Qt6 COMPONENTS Quick REQUIRED)
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
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
    SOURCES Code/AppData/tcpconnection.h Code/AppData/tcpconnection.cpp
    SOURCES Code/RDP/remotecontrolthread.h Code/RDP/remotecontrolthread.cpp
    SOURCES Code/AppData/devicedata.cpp
    SOURCES Code/TCP/tcpservertest.h Code/TCP/tcpservertest.cpp
    SOURCES Code/RDP/remoteimageprovider.h Code/RDP/remoteimageprovider.cpp
    SOURCES Code/TCP/tcpserverthread.h Code/TCP/tcpserverthread.cpp
    QML_FILES Page/RCRpage/NotificationBanner.qml
)

# 如果您不是在开发 macOS 应用程序，可以注释掉以下行
# set_target_properties(appremote PROPERTIES
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
# )

# 如果您的应用程序不是在 Windows 平台上，可以注释掉以下行
set_target_properties(appremote PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# 判断操作系统类型
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Compiling for Linux")

    # 查找 FreeRDP 和 WinPR
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FREERDP REQUIRED freerdp2)
    pkg_check_modules(WINPR REQUIRED winpr2)

    find_library(CZMQ_LIB NAMES czmq PATHS /usr/lib/x86_64-linux-gnu)
    if(NOT CZMQ_LIB)
        message(FATAL_ERROR "CZMQ library not found!")
    endif()

    # 包含头文件路径
    include_directories(${FREERDP_INCLUDE_DIRS} ${WINPR_INCLUDE_DIRS})

    # Link libraries
    target_link_libraries(appremote PRIVATE ${CZMQ_LIB})
    # 链接库
    target_link_libraries(appremote PRIVATE
        ${FREERDP_LIBRARIES} ${WINPR_LIBRARIES}
    )
    # Link Qt and CZMQ libraries
    target_link_libraries(appremote PRIVATE czmq)

    # 添加 Linux 特定的编译定义
    add_definitions(-DLINUX)
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
    find_package(ZeroMQ CONFIG REQUIRED)
    target_link_libraries(appremote PRIVATE libzmq libzmq-static)
    find_package(czmq CONFIG REQUIRED)
    target_link_libraries(appremote PRIVATE czmq czmq-static)
endif()

target_link_libraries(appremote
    PRIVATE Qt6::Quick Qt6::Sql)
target_link_libraries(appremote
    PRIVATE Qt6::Widgets)
target_link_libraries(appremote
    PRIVATE Qt6::Multimedia)

include(GNUInstallDirs)
install(TARGETS appremote
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
