cmake_minimum_required(VERSION 3.16)

project(remote VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17) # 指定 C++ 标准版本
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)

qt_add_executable(appremote
    Login.cpp
)

qt_add_qml_module(appremote
    URI remote
    VERSION 1.0
    QML_FILES Login.qml
    SOURCES globalproperties.h globalproperties.cpp
    RESOURCES image.qrc
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
    PRIVATE Qt6::Quick)

include(GNUInstallDirs)
install(TARGETS appremote
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
