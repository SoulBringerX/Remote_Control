import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Rectangle {
    id: personalSetting
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    color: "transparent"

    // ================= 1. 基础组件声明 =================
    component SettingsItem: Rectangle {
        id: root
        property alias title: label.text
        property alias icon: iconText.text
        property alias rightItem: rightLoader.sourceComponent
        signal clicked()

        implicitHeight: 50
        color: "white"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 12

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
                id: rightLoader
                active: !!root.rightItem
            }
        }

        MouseArea {
            anchors.fill: parent
            // 修改为始终使用手型光标（或根据需要设置）
            cursorShape: Qt.PointingHandCursor
            // 无条件触发 clicked 信号
            onClicked: root.clicked()
        }
    }

    component InfoItem: Rectangle {
        property alias title: titleText.text
        property alias value: valueText.text

        implicitHeight: 40
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

    // ================= 2. 功能组件声明 =================
    component PasswordDialog: Popup {
        id: dialog
        width: Math.min(parent.width * 0.8, 400)
        height: contentColumn.implicitHeight + 40
        modal: true
        dim: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        ColumnLayout {
            id: contentColumn
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15

            // Header
            RowLayout {
                spacing: 10
                Image {
                    source: "qrc:/icons/lock.svg"
                    sourceSize: Qt.size(24, 24)
                }
                Label {
                    text: "修改密码"
                    font.bold: true
                    font.pixelSize: 18
                }
            }

            // 当前密码
            TextField {
                id: currentPasswordField
                placeholderText: "当前密码"
                echoMode: TextInput.Password
                validator: RegularExpressionValidator {
                    regularExpression: /^[\w@#$%^&*]{6,20}$/
                }
            }

            // 新密码
            TextField {
                id: newPasswordField
                placeholderText: "新密码（6-20位字符）"
                echoMode: TextInput.Password
                validator: RegularExpressionValidator {
                    regularExpression: /^[\w@#$%^&*]{6,20}$/
                }
            }

            // 确认新密码
            TextField {
                id: confirmPasswordField
                placeholderText: "确认新密码"
                echoMode: TextInput.Password
                validator: RegularExpressionValidator {
                    // 如果确认密码与新密码一致，则接收任意输入，否则无效
                    regularExpression: newPasswordField.text === text ? /.*/ : /^$/
                }
            }

            // 错误信息
            Label {
                id: errorLabel
                visible: false
                color: "#e74c3c"
                font.pixelSize: 12
            }

            // 按钮行
            RowLayout {
                spacing: 15
                Layout.alignment: Qt.AlignRight

                Button {
                    text: "取消"
                    flat: true
                    onClicked: dialog.close()
                }

                Button {
                    text: "确认修改"
                    enabled: currentPasswordField.acceptableInput &&
                             newPasswordField.acceptableInput &&
                             confirmPasswordField.acceptableInput
                    onClicked: validateAndSubmit()
                }
            }
        }

        function validateAndSubmit() {
            if (newPasswordField.text !== confirmPasswordField.text) {
                showError("两次输入的密码不一致")
                return
            }

            if (newPasswordField.text === currentPasswordField.text) {
                showError("新密码不能与旧密码相同")
                return
            }

            // TODO: 调用后端接口
            console.log("密码修改请求发送...")
            showBanner("密码修改成功", "#2ecc71")
            close()
        }

        function showError(message) {
            errorLabel.text = message
            errorLabel.visible = true
            errorTimer.restart()
        }

        Timer {
            id: errorTimer
            interval: 3000
            onTriggered: errorLabel.visible = false
        }
    }

    // ================= 3. 主界面布局 =================
    ColumnLayout {
        anchors.fill: parent
        spacing: 20

        // 用户信息区域
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 150
            color: "#f5f5f5"

            RowLayout {
                anchors.centerIn: parent
                spacing: 20

                // 用户头像
                Rectangle {
                    width: 80
                    height: 80
                    radius: width / 2
                    color: "#e0e0e0"
                    Text {
                        anchors.centerIn: parent
                        text: "🖤"
                        font.pixelSize: 36
                    }
                }

                // 用户名称
                ColumnLayout {
                    spacing: 8
                    TextInput {
                        id: usernameInput
                        text: "未命名用户"
                        font.pixelSize: 24
                        maximumLength: 16
                        color: editButton.checked ? "#333" : "#666"
                        readOnly: !editButton.checked
                        Rectangle {
                            anchors.bottom: parent.bottom
                            width: parent.width
                            height: 1
                            color: "#ddd"
                            visible: editButton.checked
                        }
                    }

                    Button {
                        id: editButton
                        text: checked ? "保存修改" : "编辑昵称"
                        checkable: true
                        flat: true
                        font.pixelSize: 12
                        palette.buttonText: "#2196F3"
                        onCheckedChanged: {
                            if (!checked)
                                saveUsername()
                        }
                    }
                }
            }
        }

        // 账户安全设置
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: "账户安全"
                font.bold: true
                leftPadding: 16
                color: "#666"
            }

            SettingsItem {
                title: "修改密码"
                icon: "🔐"
                onClicked: {
                    console.log("打开修改密码弹窗")
                    passwordDialog.open()
                }
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

        // 账户信息
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: "账户信息"
                font.bold: true
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

    // ================= 4. 功能实现 =================
    PasswordDialog {
        id: passwordDialog
        anchors.centerIn: parent
    }

    function saveUsername() {
        if (usernameInput.text.trim().length < 2) {
            showBanner("用户名不能少于2个字符", "#e74c3c")
            return
        }
        // TODO: 调用保存接口
        showBanner("用户名修改成功", "#2ecc71")
    }

    function toggleSecurityLock(enable) {
        console.log("设备安全锁状态:", enable)
        // TODO: 调用安全锁接口
    }

    function showBanner(message, color) {
        var component = Qt.createComponent("qrc:/components/NotificationBanner.qml")
        if (component.status === Component.Ready) {
            var banner = component.createObject(personalSetting, {
                text: message,
                backgroundColor: color
            })
            banner.show()
        }
    }
}
