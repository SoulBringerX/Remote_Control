import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import "../Dialog/"
import "./"

Rectangle {
    id: directConnect
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    color: "transparent"

    property string errorMessage: ''

    signal deviceConnected(string ip)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

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
                    onTextChanged: {
                        const ipPattern = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/
                        if (!ipPattern.test(text)) {
                            // 可以在这里添加错误提示逻辑
                        }
                    }
                }
            }
        }

        Rectangle {
            id: connectButton
            Layout.alignment: Qt.AlignHCenter
            height: parent.height * 0.0825
            width: parent.width * 0.375
            color: "#007BFF"
            border.color: "#0056b3"
            border.width: 1
            Text {
                text: "连接"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 16
            }
            MouseArea {
                id: connectButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onHoveredChanged: {
                    if (connectButtonMouseArea.containsMouse) {
                        connectButton.color = "white"
                    } else {
                        connectButton.color = "#007BFF"
                    }
                }
                onClicked: {
                    if (tcp.connect(ipAddress.text)) {
                        console.log(ipAddress.text)
                        remoteControlThread.startConnection(ipAddress.text, username.text, password.text)
                        remoteView.show()
                        deviceConnected(ipAddress.text)
                    } else {
                        errorMessage = "TCP连接失败"
                        errorDialog.show()
                    }
                }
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "已准备好连接"
            font.pixelSize: 14
            color: "#28a745"
        }
    }

    SuecessDialog {
        id: registerDialog
    }

    SystemErrorDialog {
        id: errorDialog
        message: directConnect.errorMessage
    }

    RemoteDraw {
        id: remoteView
    }
}
