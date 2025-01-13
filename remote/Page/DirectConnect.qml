import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    id: directConnect
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    color: "transparent"



    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        // 输入远程的设备的IP地址
        TextField {
            id: ipAddress
            placeholderText: "请输入远程设备的IP地址"
            anchors.horizontalCenter: TextField.AlignHCenter
            height: 40
            font.pixelSize: 16
            background: Rectangle {
                color: "white"
                radius: 5
                border.color: "#CCCCCC"
                border.width: 1
            }
        }

        // 输入远端windows的本地账号
        TextField {
            id: username
            placeholderText: "请输入远端windows的本地账号"
            anchors.horizontalCenter: TextField.AlignHCenter
            height: 40
            font.pixelSize: 16
            background: Rectangle {
                color: "white"
                radius: 5
                border.color: "#CCCCCC"
                border.width: 1
            }
        }

        // 输入远端windows的本地密码
        TextField {
            id: password
            placeholderText: "请输入远端windows的本地密码"
            anchors.horizontalCenter: TextField.AlignHCenter
            height: 40
            font.pixelSize: 16
            background: Rectangle {
                color: "white"
                radius: 5
                border.color: "#CCCCCC"
                border.width: 1
            }
            echoMode: TextInput.Password
        }

        // 连接按钮
        Button {
            id: connectButton
            text: "连接"
            anchors.horizontalCenter: Button.AlignHCenter
            height: 40
            font.pixelSize: 16
            background: Rectangle {
                color: "#007BFF"
                radius: 5
            }
            onClicked: {
                // 连接逻辑
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // 连接逻辑
                }
                onPressed: {
                    parent.background.color = "#0056b3";
                }
                onReleased: {
                    parent.background.color = "#007BFF";
                }
            }
        }
    }
}
