cmake_minimum_required(VERSION 3.16)

project(remote VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11) # 指定 C++ 标准版本
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_NO_SHOW_OLD_POLICY_WARNINGS ON)

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
    RESOURCES resource.qrc
    QML_FILES Page/Mainwindow.qml
    SOURCES Code/Users/account.h Code/Users/account.cpp
    QML_FILES Page/Register.qml
    SOURCES Login.cpp
    QML_FILES Page/Dialog/SuecessDialog.qml
    QML_FILES Page/Main.qml
    QML_FILES Model/DeviceInformationModel.qml
    QML_FILES Model/DeviceAppModel.qml
    SOURCES Code/Database/database.h Code/Database/database.cpp
    QML_FILES Page/Dialog/SystemErrorDialog.qml
    SOURCES Code/AppData/devicedate.h
    SOURCES Code/AppData/applicationData.h
    SOURCES imageresource.h imageresource.cpp
    QML_FILES Page/UserDevices.qml
    QML_FILES Page/RemoteSetting.qml
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
