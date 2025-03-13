import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../Dialog/"

Rectangle {
    id: personalSetting
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    color: "#f0f2f5"

    ColumnLayout {
        anchors.fill: parent
        spacing: 20

        // 顶部用户信息区域
        Rectangle {
            Layout.fillWidth: true
            height: 180
            color: "#ffffff"
            border.width: 1
            border.color: "#e0e0e0"
            radius: 8

            RowLayout {
                anchors.centerIn: parent
                spacing: 30

                // 用户头像
                Rectangle {
                    width: 100
                    height: 100
                    radius: width/2
                    color: "#0066ff"
                    border.width: 2
                    border.color: "#ffffff"

                    Text {
                        anchors.centerIn: parent
                        text: "U"
                        font.pixelSize: 48
                        color: "white"
                    }
                }

                // 用户名编辑
                ColumnLayout {
                    spacing: 10

                    TextInput {
                        id: usernameInput
                        text: account.loadUsername()
                        font.pixelSize: 28
                        readOnly: !editBtn.checked
                        color: editBtn.checked ? "#333333" : "#999999"
                        selectionColor: "#0066ff" // 调整文字选中时的背景色
                        maximumLength: 16

                        Rectangle {
                            anchors.bottom: parent.bottom
                            width: parent.width
                            height: 1
                            color: editBtn.checked ? "#0066ff" : "#e0e0e0"
                            visible: editBtn.checked
                        }
                    }

                    Button {
                        id: editBtn
                        text: checked ? "保存" : "编辑"
                        checkable: true
                        font.pixelSize: 14
                        padding: 10
                        background: Rectangle {
                            color: checked ? "#0066ff" : "#f5f5f5"
                            border.width: 1
                            border.color: checked ? "#0066ff" : "#e0e0e0"
                            radius: 4
                        }
                        palette.buttonText: checked ? "#ffffff" : "#333333"

                        onCheckedChanged: if (!checked) saveUsername()
                    }
                }
            }
        }

        // 账户安全设置
        Rectangle {
            Layout.fillWidth: true
            height: 120
            color: "#ffffff"
            border.width: 1
            border.color: "#e0e0e0"
            radius: 8

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 20
                spacing: 15

                Text {
                    text: "账户安全"
                    font.bold: true
                    font.pixelSize: 18
                    leftPadding: 20
                    color: "#333333"
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    spacing: 20

                    // 重置密码按钮
                    Button {
                        id: resetPasswordBtn
                        Layout.fillWidth: true
                        font.pixelSize: 16
                        text: "重置密码"
                        onClicked: passwordDialog.open()
                        padding: 15
                        background: Rectangle {
                            color: "#f5f5f5"
                            border.width: 1
                            border.color: "#e0e0e0"
                            radius: 4
                        }
                        // 图标
                        Icon {
                            iconSource: "qrc:/icons/lock.svg" // 替换为实际图标路径
                            width: 24
                            height: 24
                            anchors.baseline: parent.baseline
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                        }
                    }

                    // 设备安全锁开关
                    Switch {
                        id: securityLockSwitch
                        Layout.fillWidth: true
                        text: "设备安全锁"
                        checked: false
                        onCheckedChanged: {
                            if (checked) {
                                // 如果开启设备安全锁，弹出配置密码对话框
                                securityLockDialog.open()
                            } else {
                                toggleSecurityLock(checked)
                            }
                        }
                    }
                }
            }
        }

        // 用户信息展示
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 15

            Text {
                text: "账户信息"
                font.bold: true
                font.pixelSize: 18
                leftPadding: 20
                color: "#333333"
            }

            Repeater {
                model: 3

                Rectangle {
                    width: parent.width
                    height: 60
                    color: "#ffffff"
                    border.width: 1
                    border.color: "#e0e0e0"
                    radius: 8

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20

                        Text {
                            text: modelData === 0 ? "注册时间" : (modelData === 1 ? "绑定邮箱" : "手机号码")
                            color: "#666666"
                            font.pixelSize: 16
                            Layout.fillWidth: true
                            verticalAlignment: Text.AlignVCenter
                        }

                        Text {
                            text: modelData === 0 ? "2023-01-01" : (modelData === 1 ? "user@example.com" : "+86 138****5678")
                            color: "#999999"
                            font.pixelSize: 16
                            Layout.alignment: Qt.AlignRight
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }
    }

    // 密码修改对话框
    Dialog {
        id: passwordDialog
        title: "修改密码"
        modal: true
        width: 400
        height: 300
        background: Rectangle {
            color: "#ffffff"
            border.width: 1
            border.color: "#e0e0e0"
            radius: 8
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.topMargin: 20
            spacing: 20

            TextField {
                id: currentPasswordField
                placeholderText: "当前密码"
                echoMode: TextInput.Password
                font.pixelSize: 16
                padding: 10
                background: Rectangle {
                    color: "#f5f5f5"
                    border.width: 1
                    border.color: "#e0e0e0"
                    radius: 4
                }
                width: parent.width * 0.8 // 统一输入框长度
            }

            TextField {
                id: newPasswordField
                placeholderText: "新密码"
                echoMode: TextInput.Password
                font.pixelSize: 16
                padding: 10
                background: Rectangle {
                    color: "#f5f5f5"
                    border.width: 1
                    border.color: "#e0e0e0"
                    radius: 4
                }
                width: parent.width * 0.8 // 统一输入框长度
            }

            TextField {
                id: confirmPasswordField
                placeholderText: "确认新密码"
                echoMode: TextInput.Password
                font.pixelSize: 16
                padding: 10
                background: Rectangle {
                    color: "#f5f5f5"
                    border.width: 1
                    border.color: "#e0e0e0"
                    radius: 4
                }
                width: parent.width * 0.8 // 统一输入框长度
            }

            RowLayout {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                anchors.right: parent.right
                anchors.rightMargin: 20
                spacing: 10

                Button {
                    text: "取消"
                    font.pixelSize: 14
                    onClicked: passwordDialog.close()
                    width: 80
                    padding: 10
                    background: Rectangle {
                        color: "#f5f5f5"
                        border.width: 1
                        border.color: "#e0e0e0"
                        radius: 4
                    }
                }

                Button {
                    text: "确定"
                    font.pixelSize: 14
                    onClicked: {
                        if (newPasswordField.text !== confirmPasswordField.text) {
                            console.log("新密码和确认密码不一致");
                            return;
                        }
                        if (newPasswordField.text.length < 6) {
                            console.log("新密码长度至少为6位");
                            return;
                        }
                        console.log("修改密码: 当前密码 =", currentPasswordField.text, "新密码 =", newPasswordField.text);
                        changePassword(currentPasswordField.text, newPasswordField.text);
                        passwordDialog.close();
                    }
                    width: 80
                    padding: 10
                    background: Rectangle {
                        color: "#0066ff"
                        border.width: 1
                        border.color: "#0066ff"
                        radius: 4
                    }
                    palette.buttonText: "#ffffff"
                }
            }
        }
    }

    // 图标组件
    component Icon: Image {
        property string iconSource
        source: iconSource
        fillMode: Image.PreserveAspectFit
    }

    function saveUsername() {
        if (usernameInput.text.trim().length < 2) {
            // 显示错误提示
            return;
        }
        account.saveNewUsername(usernameInput.text);
    }

    function toggleSecurityLock(enable) {
        // 设备安全锁逻辑
        if (enable) {
            // 开启设备安全锁的逻辑
            console.log("设备安全锁已开启");
        } else {
            // 关闭设备安全锁的逻辑
            console.log("设备安全锁已关闭");
        }
    }

    function changePassword(oldPassword, newPassword) {
        if (account.changePassWord(oldPassword, newPassword)) {
            suecessDialog.message = "修改密码成功"
            suecessDialog.show();
        } else {
            systemErrorDialog.message = "CE_02:用户输入的旧密码不对"
            systemErrorDialog.show();
        }
    }

    function setSecurityLockPassword(password) {
        // 在这里调用你的 C++ 函数来设置设备安全锁密码
        console.log("设置设备安全锁密码: ", password);
    }

    SystemErrorDialog {
        id: systemErrorDialog
    }

    SuecessDialog {
        id: suecessDialog
    }
    LockDialog{
        id:securityLockDialog
    }
}
