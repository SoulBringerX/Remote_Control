import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: deviceLockDialog
    property string deviceName: ""
    property int deviceIndex: -1
    property string correctPassword: ""

    signal unlockConfirmed(int index)
    signal deleteConfirmed(int index)

    title: deviceName + " - 安全锁验证"
    modal: true
    width: 350
    height: 220
    standardButtons: Dialog.NoButton
    visible: false

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Text {
            text: "请输入安全锁密码以解锁设备"
            font.pointSize: 14
            wrapMode: Text.WordWrap
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: passwordInput
            placeholderText: "输入密码"
            echoMode: TextInput.Password
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: 10

            Button {
                text: "确认解锁"
                onClicked: {
                    if (account.checkSecurityLockPassword(passwordInput.text)) {
                        deviceLockDialog.close()
                    } else {
                        passwordInput.placeholderText = "密码错误，请重试"
                    }
                }
            }

            Button {
                text: "取消"
                onClicked: deviceLockDialog.close()
            }

            Button {
                text: "删除设备"
                onClicked: {
                    deleteConfirmed(deviceIndex)
                    deviceLockDialog.close()
                }
                background: Rectangle {
                    color: "#f44336"
                    radius: 4
                }
                contentItem: Text {
                    text: "删除设备"
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                }
            }
        }
    }

    onVisibleChanged: {
        if (visible) {
            passwordInput.text = ""
        }
    }
}
