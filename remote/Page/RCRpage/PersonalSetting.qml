import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../Dialog/"

Rectangle {
    id: personalSetting
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    color: "#f0f2f5"

    property bool checked;

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
                        text: Account.loadUsername()
                        font.pixelSize: 28
                        readOnly: !editBtn.checked
                        color: editBtn.checked ? "#333333" : "#999999"
                        selectionColor: "#0066ff"
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
            height: 160 // 增加高度以容纳新按钮
            color: "#ffffff"
            border.width: 1
            border.color: "#e0e0e0"
            radius: 8
            visible: Account.isOnline ?  true : false;

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
                        Icon {
                            iconSource: "qrc:/icons/lock.svg"
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
                        checked: Account.checkIsLocked();
                        onCheckedChanged: {
                            Account.saveSecurityLock(checked)
                        }
                    }
                }

                // 修改设备安全锁密码按钮
                Button {
                    id: changeSecurityLockPasswordBtn
                    Layout.fillWidth: true
                    font.pixelSize: 16
                    text: "修改设备安全锁密码"
                    onClicked: securityLockDialog.open()
                    padding: 15
                    background: Rectangle {
                        color: "#f5f5f5"
                        border.width: 1
                        border.color: "#e0e0e0"
                        radius: 4
                    }
                    Icon {
                        iconSource: "qrc:/icons/key.svg" // 替换为实际图标路径
                        width: 24
                        height: 24
                        anchors.baseline: parent.baseline
                        anchors.left: parent.left
                        anchors.leftMargin: 10
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
                width: parent.width * 0.8
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
                width: parent.width * 0.8
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
                width: parent.width * 0.8
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

    // 设备安全锁密码修改对话框
    Dialog {
        id: securityLockDialog
        title: "修改设备安全锁密码"
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
                id: currentSecurityLockPasswordField
                placeholderText: "当前设备安全锁密码"
                echoMode: TextInput.Password
                font.pixelSize: 16
                padding: 10
                background: Rectangle {
                    color: "#f5f5f5"
                    border.width: 1
                    border.color: "#e0e0e0"
                    radius: 4
                }
                width: parent.width * 0.8
            }

            TextField {
                id: newSecurityLockPasswordField
                placeholderText: "新设备安全锁密码"
                echoMode: TextInput.Password
                font.pixelSize: 16
                padding: 10
                background: Rectangle {
                    color: "#f5f5f5"
                    border.width: 1
                    border.color: "#e0e0e0"
                    radius: 4
                }
                width: parent.width * 0.8
            }

            TextField {
                id: confirmSecurityLockPasswordField
                placeholderText: "确认新设备安全锁密码"
                echoMode: TextInput.Password
                font.pixelSize: 16
                padding: 10
                background: Rectangle {
                    color: "#f5f5f5"
                    border.width: 1
                    border.color: "#e0e0e0"
                    radius: 4
                }
                width: parent.width * 0.8
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
                    onClicked: securityLockDialog.close()
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
                        if (newSecurityLockPasswordField.text !== confirmSecurityLockPasswordField.text) {
                            console.log("新设备安全锁密码和确认密码不一致");
                            return;
                        }
                        if (newSecurityLockPasswordField.text.length < 6) {
                            console.log("新设备安全锁密码长度至少为6位");
                            return;
                        }
                        console.log("修改设备安全锁密码: 当前密码 =", currentSecurityLockPasswordField.text, "新密码 =", newSecurityLockPasswordField.text);
                        changeSecurityLockPassword(currentSecurityLockPasswordField.text, newSecurityLockPasswordField.text);
                        securityLockDialog.close();
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

    function changePassword(oldPassword, newPassword) {
        if (account.changePassWord(oldPassword, newPassword)) {
            suecessDialog.message = "修改密码成功"
            suecessDialog.show();
        } else {
            systemErrorDialog.message = "CE_02:用户输入的旧密码不对"
            systemErrorDialog.show();
        }
    }

    function changeSecurityLockPassword(oldPassword, newPassword) {
        // 在这里调用你的 C++ 函数来修改设备安全锁密码
        console.log("修改设备安全锁密码: 旧密码 =", oldPassword, "新密码 =", newPassword);
        account.saveSecurityLockPassword(oldPassword, newPassword);
    }

    SystemErrorDialog {
        id: systemErrorDialog
    }

    SuecessDialog {
        id: suecessDialog
    }
}
