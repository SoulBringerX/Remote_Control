import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: remoteSetting
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    color: "transparent"

    ScrollView {
        anchors.fill: parent
        clip: false
        ColumnLayout {
            width: parent.width
            spacing: 20

            // bpp 配置选项
            Rectangle {
                width: parent.width
                height: 100
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    ComboBox {
                        width: parent.width * 0.5
                        model: ["8 bpp - 最低画质", "16 bpp - 较低画质", "24 bpp - 中等画质", "32 bpp - 最高画质"]
                    }
                    Text {
                        text: "选择位图缓冲深度以调整画面效果和流畅度。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }

            // sound 配置选项
            Rectangle {
                width: parent.width
                height: 80
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    CheckBox { text: "启用音频输出 (sys:pulse, latency:100)" }
                    Text {
                        text: "启用音频输出并设置延迟为100ms。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }

            // microphone 配置选项
            Rectangle {
                width: parent.width
                height: 80
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    CheckBox { text: "启用麦克风输入 (sys:pulse)" }
                    Text {
                        text: "启用麦克风音频输入。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }

            // rfx 配置选项
            Rectangle {
                width: parent.width
                height: 80
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    CheckBox { text: "启用 RemoterFX" }
                    Text {
                        text: "启用 RemoterFX 显示特性以提高图像质量。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }

            // gdi 配置选项
            Rectangle {
                width: parent.width
                height: 100
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    ComboBox {
                        width: parent.width * 0.5
                        model: ["硬件模式 (hw)", "软件模式 (sw)"]
                    }
                    Text {
                        text: "选择画面绘制模式：硬件模式适用于终端，软件模式适用于远程服务器。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }

            // drives 配置选项
            Rectangle {
                width: parent.width
                height: 80
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    CheckBox { text: "启用本地磁盘映射" }
                    Text {
                        text: "将本地磁盘映射到远程会话中。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }

            // video 配置选项
            Rectangle {
                width: parent.width
                height: 80
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    CheckBox { text: "开启视频增强" }
                    Text {
                        text: "启用视频增强功能以提高视频播放质量。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }

            // usb 配置选项
            Rectangle {
                width: parent.width
                height: 80
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    CheckBox { text: "开启 USB 重定向" }
                    Text {
                        text: "启用 USB 重定向，支持 U 盘、打印机等设备。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }

            // gfx 配置选项
            Rectangle {
                width: parent.width
                height: 80
                color: "lightgray"
                radius: 5
                border.color: "darkgray"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    CheckBox { text: "启用 AVC/H264 编码" }
                    Text {
                        text: "启用 AVC/H264 编码以提高视频编码效率。"
                        wrapMode: Text.WordWrap
                        color: "gray"
                    }
                }
            }
        }
    }
}