import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    radius: 8
    color: "#fafafa"
    border.color: "#ddd"
    border.width: 1

    ScrollView {
        width: parent.width
        height: parent.height
        anchors.fill: parent
        clip: true

        ColumnLayout {
            id: mainLayout
            anchors.margins: 16
            spacing: 16
            width: parent.width

            // 标题 (Title)
            Text {
                text: qsTr("远程连接设置")
                font.bold: true
                font.pixelSize: 20
                color: "#333"
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            // 分割线 (Divider)
            Rectangle {
                height: 1
                color: "#ddd"
                width: parent.width
                Layout.fillWidth: true
            }

            // ======= 基本设置 (Basic Settings) =======
            Text {
                text: qsTr("基本设置")
                font.bold: true
                color: "#333"
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                spacing: 8
                Layout.fillWidth: true
                Label {
                    text: qsTr("位图缓冲深度")
                    Layout.alignment: Qt.AlignRight
                    Layout.preferredWidth: 120
                }
                ComboBox {
                    id: bppComboBox
                    Layout.fillWidth: true
                    model: [
                        "8 bpp - 最低画质",
                        "16 bpp - 较低画质",
                        "24 bpp - 中等画质",
                        "32 bpp - 最高画质"
                    ]
                    onActivated: client.setBpp(currentIndex)
                }
            }

            // 分割线 (Divider)
            Rectangle {
                height: 1
                width: parent.width
                color: "#ddd"
                Layout.fillWidth: true
            }

            // ======= 高级设置 (Advanced Settings) =======
            Text {
                text: qsTr("高级设置")
                font.bold: true
                color: "#333"
                Layout.alignment: Qt.AlignHCenter
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                CheckBox {
                    id: audioCheckBox
                    text: qsTr("启用音频输出")
                    onCheckedChanged: client.setAudioEnabled(checked)
                }
                CheckBox {
                    id: micCheckBox
                    text: qsTr("启用麦克风输入")
                    onCheckedChanged: client.setMicrophoneEnabled(checked)
                }
                CheckBox {
                    id: remoteFxCheckBox
                    text: qsTr("启用 RemoteFX")
                    onCheckedChanged: client.setRemoteFxEnabled(checked)
                }
                CheckBox {
                    id: nsCodecCheckBox
                    text: qsTr("启用 NSCodec 编码")
                    onCheckedChanged: client.setNSCodecEnabled(checked)
                }
                CheckBox {
                    id: surfaceCommandsCheckBox
                    text: qsTr("启用 Surface Commands")
                    onCheckedChanged: client.setSurfaceCommandsEnabled(checked)
                }
                CheckBox {
                    id: remoteConsoleAudioCheckBox
                    text: qsTr("启用远程控制音频")
                    onCheckedChanged: client.setRemoteConsoleAudioEnabled(checked)
                }
                CheckBox {
                    id: driveMappingCheckBox
                    text: qsTr("启用本地磁盘映射（需打开设备重定向）")
                    onCheckedChanged: client.setDriveMappingEnabled(checked)
                }
                CheckBox {
                    id: usbRedirectionCheckBox
                    text: qsTr("开启设备重定向")
                    onCheckedChanged: client.setUsbRedirectionEnabled(checked)
                }
                CheckBox {
                    id: clipboardRedirectionCheckBox
                    text: qsTr("启用剪贴板重定向（需打开设备重定向）")
                    onCheckedChanged: client.setClipboardRedirectionEnabled(checked)
                }
            }

            // 分割线 (Divider)
            Rectangle {
                height: 1
                color: "#ddd"
                Layout.fillWidth: true
            }

            // 保存按钮 (Save Button)
            Button {
                text: qsTr("保存设置")
                Layout.alignment: Qt.AlignHCenter
                onClicked: client.saveSettings()
            }
        }
    }

    // 初始化时加载配置 (Initialize and Load Settings)
    Component.onCompleted: {
        console.log("初始化 RDP 连接...");
        if (!client.initialize()) {
            console.error("RDP 初始化失败！");
        } else {
            console.log("RDP 初始化成功！");
            console.log("加载用户配置...");
            client.loadSettings();  // 加载配置
            updateUI();  // 更新 UI
        }
    }

    // 更新 UI 的函数 (Function to Update UI)
    function updateUI() {
        bppComboBox.currentIndex = client.getBppIndex();
        audioCheckBox.checked = client.getAudioEnabled();
        micCheckBox.checked = client.getMicrophoneEnabled();
        remoteFxCheckBox.checked = client.getRemoteFxEnabled();
        nsCodecCheckBox.checked = client.getNSCodecEnabled();
        surfaceCommandsCheckBox.checked = client.getSurfaceCommandsEnabled();
        remoteConsoleAudioCheckBox.checked = client.getRemoteConsoleAudioEnabled();
        driveMappingCheckBox.checked = client.getDriveMappingEnabled();
        usbRedirectionCheckBox.checked = client.getUsbRedirectionEnabled();
        clipboardRedirectionCheckBox.checked = client.getClipboardRedirectionEnabled();
    }
}
