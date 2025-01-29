import QtQuick
import QtQuick.Window
import QtQuick.Controls

import "./Dialog/"
Window {
    id: root
    maximumWidth: Screen.desktopAvailableWidth * 0.265
    maximumHeight: Screen.desktopAvailableHeight * 0.555
    minimumWidth: Screen.desktopAvailableWidth * 0.265
    minimumHeight: Screen.desktopAvailableHeight * 0.555
    visible: true
    title: qsTr("欢迎使用RemoteControl")
    flags: Qt.platform.os === "linux" ? Qt.Window | Qt.WindowCloseButtonHint : Qt.FramelessWindowHint | Qt.Window | Qt.WindowCloseButtonHint

    property string errorMessage:''

    Item {
        id: registerPage
        anchors.fill: parent // 填充整个窗口
        Image {
            id: windowBackground
            source: "qrc:/images/back1.jpg"
            anchors.fill: parent
        }
        Rectangle
        {
            id: loginPageBackRectangle
            height: 32
            width: height*3
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.top: parent.top
            anchors.topMargin : Qt.platform.os === "linux" ? parent.height * 0.01 :parent.height * 0.035
            color: "transparent"
            
            Image{
                id: backIcon
                height: 18
                width: height
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/images/back1.svg"
            }
            Text
            {
                color: "white"
                text: qsTr("返回登录")
                font.pointSize: 12
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: backIcon.right
                anchors.leftMargin: 5
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked:{
                    loader.sourceComponent = loginPage
                }
            }
        }
        Rectangle
        {
            id: themeColorChangedRectangle
            width: 69
            height: 32
            color: "lightblue"
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.top: parent.top
            anchors.topMargin : Qt.platform.os === "linux" ? parent.height * 0.01 :parent.height * 0.035
            radius: height / 2

            //加入动画以更改登录界面的主题
            Rectangle
            {
                id: themeColorIconRectangle
                width: 32
                height: 32
                radius: width / 2
                x: 0

                Image
                {
                    id: themeIcon
                    source: "qrc:/images/sun.svg"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                }
                MouseArea {
                    property bool iconchanged: true;
                    anchors.fill: parent
                    onClicked: {
                        // 触发动画
                        if(iconchanged)
                        {
                            themeColorChangedAnimation1.running = true
                            iconchanged = false
                            themeIcon.source = "qrc:/images/moon.svg"
                            windowBackground.source = "qrc:/images/back1.jpg"
                            themeColorChanged1.running = true
                        }
                        else
                        {
                            themeColorChangedAnimation2.running = true
                            iconchanged = true
                            themeIcon.source = "qrc:/images/sun.svg"
                            windowBackground.source = "qrc:/images/sunback.jpg"
                            themeColorChanged2.running = true
                        }
                   }
               }

               PropertyAnimation {
                   id: themeColorChangedAnimation1
                   target: themeColorIconRectangle
                   properties: "x"
                   from: 0
                   to: 37
                   duration: 500 // 动画持续时间，以毫秒为单位
                   easing.type: Easing.InOutQuad // 缓动函数
               }

               PropertyAnimation {
                   id: themeColorChangedAnimation2
                   target: themeColorIconRectangle
                   properties: "x"
                   from: 37
                   to: 0
                   duration: 500 // 动画持续时间，以毫秒为单位
                   easing.type: Easing.InOutQuad // 缓动函数
               }
               //此处为切换背景图
               PropertyAnimation
               {
                   id: themeColorChanged1
                   target: themeColorChangedRectangle
                   properties: "color"
                   from: "lightblue"
                   to: "#0b0b4d"
                   duration: 500
               }
               PropertyAnimation
               {
                   id: themeColorChanged2
                   target: themeColorChangedRectangle
                   properties: "color"
                   from: "#0b0b4d"
                   to: "lightblue"
                   duration: 500
               }
            }
        }
        Rectangle
        {
            id: sidBar
            width: 64
            height: width
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.2
            anchors.horizontalCenter: parent.horizontalCenter
            color: "lightblue"
            radius: height / 2

            MouseArea
            {
                id:sidBarArea
                anchors.fill:parent
                onClicked:{
                    logger.print("Register页面","打开资源管理器")
                    account.openFileManager()
                }
            }
        }
        // 账号输入框
        Rectangle {
            id: userAccountRectangle
            width: parent.width * 0.625  // 使用百分比设置输入框宽度
            height: parent.height * 0.05
            anchors.top: sidBar.bottom
            anchors.topMargin: parent.height * 0.1
            anchors.horizontalCenter: parent.horizontalCenter  // 居中
            color: "transparent"

            Rectangle {
                width: parent.width
                height: parent.height
                color: "white"

                Image {
                    id: accountIcon
                    source: "qrc:/images/Account.svg"
                    width: parent.height * 0.8
                    height: parent.height * 0.8
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                }

                TextInput {
                    id: userAccountInput
                    width: parent.width
                    readOnly: false
                    font.pixelSize: 18
                    selectionColor: "lightblue" // 选中文本的背景色
                    selectedTextColor: "white" // 选中文本的颜色
                    Keys.onEnterPressed: userAccountInput.focus = false // 按下Enter键时取消焦点
                    Keys.onReturnPressed: userAccountInput.focus = false // 按下Return键时取消焦点
                    clip: true //设置组件中文字不得超出长度
                    anchors.left: parent.left
                    anchors.leftMargin: accountIcon.width + parent.width * 0.03 // 留出图标的空间
                }
            }
        }

        // 密码输入框
        Rectangle {
            id: userPasswordRectangle
            width: parent.width * 0.625  // 使用百分比设置输入框宽度
            height: parent.height * 0.05
            anchors.top: userAccountRectangle.bottom
            anchors.topMargin: parent.height * 0.1
            anchors.horizontalCenter: parent.horizontalCenter  // 居中
            color: "transparent"

            Rectangle {
                width: parent.width
                height: parent.height
                color: "white"

                Image {
                    id: passwordIcon
                    source: "qrc:/images/password.svg"
                    width: parent.height * 0.8
                    height: parent.height * 0.8
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                }

                TextInput {
                    id: userPasswordInput
                    width: parent.width
                    readOnly: false
                    font.pixelSize: 18
                    echoMode: TextInput.Password
                    selectionColor: "lightblue" // 选中文本的背景色
                    selectedTextColor: "white" // 选中文本的颜色
                    Keys.onEnterPressed: userPasswordInput.focus = false // 按下Enter键时取消焦点
                    Keys.onReturnPressed: userPasswordInput.focus = false // 按下Return键时取消焦点
                    clip: true //设置组件中文字不得超出长度
                    anchors.left: parent.left
                    anchors.leftMargin: passwordIcon.width + parent.width * 0.03 // 留出图标的空间
                }
            }
        }

        Rectangle
        {
            id: registerButtonRectangle
            width: parent.width * 0.35
            height: parent.height * 0.0785
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: userPasswordRectangle.bottom
            anchors.topMargin: root.height * 0.09
            color: "white"
            border.color: "#26C2FA"
            border.width: 0.5

            Text
            {
                id: registerButtonText
                anchors.centerIn: parent
                text: qsTr("确认注册")
                color: "#26C2FA"
                font.pixelSize: 12
            }

            MouseArea
            {
                id: registerButtonArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if(account.registerCheck(userAccountInput.text,userPasswordInput.text)) {
                        if(account.userRegister(userAccountInput.text,userPasswordInput.text)) {
                            registerDialog.show()
                        } else {
                            errorMessage = "CE_05 账户失败"
                            userAccountInput.clear()
                            userPasswordInput.clear()
                            errorDialog.show()
                        }
                    } else {
                        errorMessage = "CE_04 账户已存在"
                        userAccountInput.clear()
                        userPasswordInput.clear()
                        errorDialog.show()
                    }
                    // 打桩测试
                    // registerDialog.show()
                }
                onHoveredChanged: {
                    if (registerButtonArea.containsMouse)
                    {
                        registerButtonRectangle.color = "#26C2FA"
                        registerButtonText.color = "white"
                    }
                    else
                    {
                        registerButtonRectangle.color = "white"
                        registerButtonText.color = "#26C2FA"
                    }
                }
            }
        }
        // 关闭按钮
        Rectangle {
            width: parent.height * 0.03
            height: parent.height * 0.03
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.01
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.01
            z: 1 // 确保关闭按钮在最上层
            color:"transparent"
            visible: Qt.platform.os === "linux" ? false : true

            Image {
                id: closeDialogImage
                source: "qrc:/images/close.svg"
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                fillMode: Image.PreserveAspectFit
                rotation: closeDialogImage.rotation

                // 使用 Behavior 使旋转平滑
                Behavior on rotation {
                    RotationAnimation {
                        duration: 300 // 动画持续时间为300ms
                        easing.type: Easing.InOutQuad
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    Qt.quit()
                }
                // 鼠标悬停时触发旋转动画
                onHoveredChanged: {
                    if (containsMouse) {
                        closeDialogImage.rotation = 180
                    } else {
                        closeDialogImage.rotation = 0
                    }
                }
            }
        }
    }
    // 监听窗口关闭事件
    onClosing: {
        // 调用 Qt.quit() 来终止程序
        Qt.quit();
    }
    SuecessDialog{
        id:registerDialog
    }
    SystemErrorDialog{
        id:errorDialog
        message: root.errorMessage
    }
}
