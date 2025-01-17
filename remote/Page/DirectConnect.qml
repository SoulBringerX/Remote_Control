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
        spacing: 20

        // 输入远程Windows的本地账号
        RowLayout {
            spacing: 10
            Rectangle {
                width: 24
                height: 24
                Image {
                    source: "qrc:/images/Account.svg"
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                }
            }
            Text {
                text: "本地账号:"
                font.pixelSize: 16
                verticalAlignment: Text.AlignVCenter
            }
            Rectangle {
                width: directConnect.width * 0.6
                height: 25
                border.color: username.focus ? "#007BFF" : "#CCCCCC"
                border.width: 2
                color: "white"
                TextField {
                    id: username
                    placeholderText: "请输入远程Windows的本地账号"
                    height: parent.height
                    font.pixelSize: 16
                    anchors.fill: parent
                }
            }
        }

        // 输入远程Windows的本地密码
        RowLayout {
            spacing: 10
            Rectangle {
                width: 24
                height: 24
                Image {
                    source: "qrc:/images/password.svg"
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                }
            }
            Text {
                text: "本地密码:"
                font.pixelSize: 16
                verticalAlignment: Text.AlignVCenter
            }
            Rectangle {
                width: directConnect.width * 0.6
                height: 25
                border.color: password.focus ? "#007BFF" : "#CCCCCC"
                border.width: 2
                color: "white"
                TextField {
                    id: password
                    placeholderText: "请输入远程Windows的本地密码"
                    height: parent.height
                    font.pixelSize: 16
                    anchors.fill: parent
                    echoMode: TextInput.Password
                }
            }
        }

        // 输入远端设备的IP地址
        RowLayout {
            spacing: 10
            Rectangle {
                width: 24
                height: 24
                Image {
                    source: "qrc:/images/ip.svg"
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                }
            }
            Text {
                text: "远端设备IP:"
                font.pixelSize: 16
                verticalAlignment: Text.AlignVCenter
            }
            Rectangle {
                width: directConnect.width * 0.6
                height: 25
                border.color: ipAddress.focus ? "#007BFF" : "#CCCCCC"
                border.width: 2
                color: "white"
                TextField {
                    id: ipAddress
                    placeholderText: "请输入远端设备的IP地址"
                    height: parent.height
                    font.pixelSize: 16
                    anchors.fill: parent
                    // 正则表达式验证
                    onTextChanged: {
                        const ipPattern = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
                        if (!ipPattern.test(text)) {
                            // 可以在这里添加错误提示逻辑
                        }
                    }
                }
            }
        }

        // 连接按钮
        Button {
            id: connectButton
            text: "连接"
            height: 50
            width: directConnect.width * 0.5
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 16
            background: Rectangle {
                color: "#007BFF"
                border.color: "#0056b3"
                border.width: 1
            }
            onClicked: {
                // 连接逻辑
            }
        }

        // 状态提示
        Text {
            text: "已准备好连接"
            font.pixelSize: 14
            color: "#28a745"
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
