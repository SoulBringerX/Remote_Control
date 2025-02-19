import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        spacing: 20

        // 顶部用户信息区域
        Rectangle {
            Layout.fillWidth: true
            height: 150
            color: "#f5f5f5"
            anchors.top: parent.top

            RowLayout {
                anchors.centerIn: parent
                spacing: 20

                // 用户头像
                Rectangle {
                    width: 80
                    height: 80
                    radius: width/2
                    color: "#e0e0e0"

                    Text {
                        anchors.centerIn: parent
                        text: "🖤"
                        font.pixelSize: 36
                    }
                }

                // 用户名编辑
                ColumnLayout {
                    TextInput {
                        id: usernameInput
                        text: "用户昵称"
                        font.pixelSize: 24
                        readOnly: !editBtn.checked
                        color: editBtn.checked ? "#333" : "#666"
                        maximumLength: 16

                        Rectangle {
                            anchors.bottom: parent.bottom
                            width: parent.width
                            height: 1
                            color: "#ddd"
                            visible: editBtn.checked
                        }
                    }

                    Button {
                        id: editBtn
                        text: checked ? "保存" : "编辑"
                        checkable: true
                        flat: true
                        font.pixelSize: 12
                        palette.buttonText: "#2196F3"

                        onCheckedChanged: if (!checked) saveUsername()
                    }
                }
            }
        }

        // 账户安全设置
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "账户安全"
                font.bold: true
                font.pixelSize: 16
                leftPadding: 16
                color: "#666"
            }

            SettingsItem {
                title: "重置密码"
                icon: "🔒"
                // onClicked: passwordDialog.open()
            }

            SettingsItem {
                title: "设备安全锁"
                icon: "📱"
                rightItem: Switch {
                    checked: false
                    onCheckedChanged: toggleSecurityLock(checked)
                }
            }
        }

        // 用户信息展示
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "账户信息"
                font.bold: true
                font.pixelSize: 16
                leftPadding: 16
                color: "#666"
            }

            InfoItem {
                title: "注册时间"
                value: "2023-01-01"
            }

            InfoItem {
                title: "绑定邮箱"
                value: "user@example.com"
            }

            InfoItem {
                title: "手机号码"
                value: "+86 138****5678"
            }
        }
    }

    // 密码修改对话框
    Dialog {
        id: passwordDialog
        title: "修改密码"
        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout {
            spacing: 10

            TextField {
                placeholderText: "当前密码"
                echoMode: TextInput.Password
            }

            TextField {
                placeholderText: "新密码"
                echoMode: TextInput.Password
            }

            TextField {
                placeholderText: "确认新密码"
                echoMode: TextInput.Password
            }
        }
    }

    // 自定义组件：设置项
    component SettingsItem: Rectangle {
        property alias title: label.text
        property alias icon: iconText.text
        property Item rightItem

        width: parent.width
        height: 50
        color: "white"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 15

            Text {
                id: iconText
                font.pixelSize: 20
            }

            Text {
                id: label
                font.pixelSize: 16
                color: "#333"
                Layout.fillWidth: true
            }

            Loader {
                sourceComponent: rightItem
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: parent.clicked()
            enabled: !!parent.rightItem
        }
    }

    // 自定义组件：信息展示项
    component InfoItem: Rectangle {
        property alias title: titleText.text
        property alias value: valueText.text

        width: parent.width
        height: 40
        color: "white"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16

            Text {
                id: titleText
                color: "#666"
                font.pixelSize: 14
            }

            Text {
                id: valueText
                color: "#999"
                font.pixelSize: 14
                Layout.alignment: Qt.AlignRight
            }
        }
    }

    function saveUsername() {
        if(usernameInput.text.trim().length < 2) {
            // 显示错误提示
            return
        }
        // 调用保存接口...
    }

    function toggleSecurityLock(enable) {
        // 安全锁逻辑...
    }
}
