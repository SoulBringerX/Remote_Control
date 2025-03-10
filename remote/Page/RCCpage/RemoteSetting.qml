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
            spacing: 10

            // ** 位图缓冲深度 (ComboBox) **
            Rectangle {
                width: parent.width
                height: 80
                color: "#f5f5f5"
                radius: 8
                border.color: "#ccc"
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 5
                    Label {
                        text: "位图缓冲深度"
                        color: "#333"
                    }
                    ComboBox {
                        width: parent.width * 0.6
                        model: ["8 bpp - 最低画质", "16 bpp - 较低画质", "24 bpp - 中等画质", "32 bpp - 最高画质"]
                        onActivated: client.setBpp(currentIndex)
                    }
                }
            }

            // ** 其他设置 (CheckBox) **
            Repeater {
                model: [
                    { text: "启用音频输出", method: "setAudioEnabled" },
                    { text: "启用麦克风输入", method: "setMicrophoneEnabled" },
                    { text: "启用 RemoterFX", method: "setRemoteFxEnabled" },
                    { text: "启用 NSCodec 编码", method: "setNSCodecEnabled" },
                    { text: "启用 Surface Commands", method: "setSurfaceCommandsEnabled" },
                    { text: "启用远程控制音频", method: "setRemoteConsoleAudioEnabled" },
                    { text: "启用本地磁盘映射", method: "setDriveMappingEnabled" },
                    { text: "开启 USB 重定向", method: "setUsbRedirectionEnabled" }
                ]

                delegate: Rectangle {
                    width: parent.width
                    height: 50
                    color: "#ffffff"
                    radius: 8
                    border.color: "#ddd"
                    border.width: 1
                    RowLayout {
                        anchors.fill: parent
                        spacing: 10
                        CheckBox {
                            text: modelData.text
                            checked: false
                            onCheckedChanged: client[modelData.method](checked)
                        }
                    }
                }
            }

            // ** 保存按钮 **
            Button {
                text: "保存设置"
                Layout.alignment: Qt.AlignHCenter
                onClicked: client.saveSettings()
            }
        }
    }
}
