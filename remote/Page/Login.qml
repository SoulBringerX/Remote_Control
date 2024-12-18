import QtQuick
import QtQuick.Window
import QtQuick.Controls


Window {
    id: loginWindow
    maximumWidth: Screen.desktopAvailableWidth * 0.675
    maximumHeight: Screen.desktopAvailableHeight * 0.625
    minimumWidth: Screen.desktopAvailableWidth * 0.675
    minimumHeight: Screen.desktopAvailableHeight * 0.625
    visible: true
    title: qsTr("欢迎使用RemoteControl")
    Item {
        id: loginPage
        anchors.fill: parent // 填充整个窗口
        Image {
            id: windowBackground
            source: "qrc:/images/sunback.jpg"
            anchors.fill: parent
        }
        Rectangle
        {
            id: loginRectangle
            height: parent.height
            width: parent.width*0.35
            color: "#E4F6FD"

            Rectangle
            {
                id: themeColorChangedRectangle
                width: 69
                height: 32
                color: "lightblue"
                anchors.right: parent.right
                anchors.rightMargin: 5
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
                                windowBackground.source = "qrc:/images/moonback.jpg"
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
                height: 64
                anchors.top: parent.top
                anchors.topMargin: 65
                anchors.horizontalCenter: parent.horizontalCenter
                color: "lightblue"
                radius: height / 2
                border.width: 2
                border.color: "#AFE6FA"
            }
            Rectangle
            {
                id: userAccountRectangle
                width: parent.width*0.7
                height: parent.height*0.0575
                anchors.top: sidBar.bottom
                anchors.left: loginWindow.left
                anchors.leftMargin: 90
                anchors.topMargin: loginWindow.height * 0.0875
                color: "transparent"

                Text{
                    id: accountText
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width*0.1
                    color: "black"
                    text: qsTr("帐号：")
                    font.pixelSize: 16
                }

                Rectangle
                {
                    width: parent.width*0.9
                    height: parent.height
                    border.color: "black"
                    border.width: 0.5
                    anchors.left: accountText.right
                    anchors.leftMargin: 10

                    TextInput
                    {
                        id: userAccountInput
                        width: parent.width*0.95
                        anchors.fill: parent
                        anchors.left: parent
                        anchors.leftMargin: 2.5
                        anchors.horizontalCenter: parent.horizontalCenter
                        readOnly: false
                        font.pixelSize: 16
                        selectionColor: "lightblue" // 选中文本的背景色
                        selectedTextColor: "white" // 选中文本的颜色
                        Keys.onEnterPressed: userPasswordInput.focus = false // 按下Enter键时取消焦点
                        Keys.onReturnPressed: userPasswordInput.focus = false // 按下Return键时取消焦点
                        clip: true //设置组件中文字不得超出长度
                    }
                }
            }
            Rectangle
            {
                id: userPasswordRectangle
                width: parent.width*0.7
                height: parent.height*0.0575
                anchors.top: userAccountRectangle.bottom
                anchors.left: loginWindow.left
                anchors.leftMargin: 90
                anchors.topMargin: loginWindow.height * 0.1
                color: "transparent"

                Text{
                    id: passwordText
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
                    color: "black"
                    text: qsTr("密码：")
                    font.pixelSize: 16
                }

                Rectangle
                {
                    width: parent.width * 0.9
                    height: parent.height
                    border.color: "black"
                    border.width: 0.5
                    anchors.left: passwordText.right
                    anchors.leftMargin: 10

                    TextInput
                    {
                        id: userPasswordInput
                        width: parent.width*0.95
                        anchors.fill: parent
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.left: parent
                        anchors.leftMargin: 2.5
                        readOnly: false
                        font.pixelSize: 16
                        echoMode: TextInput.Password
                        selectionColor: "lightblue" // 选中文本的背景色
                        selectedTextColor: "white" // 选中文本的颜色
                        Keys.onEnterPressed: userPasswordInput.focus = false // 按下Enter键时取消焦点
                        Keys.onReturnPressed: userPasswordInput.focus = false // 按下Return键时取消焦点
                        clip: true //设置组件中文字不得超出长度
                    }
                }
            }
            Rectangle
            {
                id: loginButtonRectangle
                width: parent.width * 0.565
                height: parent.height * 0.0785
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: userPasswordRectangle.bottom
                anchors.topMargin: loginWindow.height * 0.09
                color: "white"
                border.color: "#26C2FA"
                border.width: 0.5

                Text {
                    id: loginButtonText
                    anchors.centerIn: parent
                    text: qsTr("登录")
                    color: "#26C2FA"
                    font.pixelSize: 12
                }

                MouseArea {
                    id: loginButtonArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked:
                    {
                        //登录逻辑
                        // if(account.loginCheck(userAccountInput.text,userPasswordInput.text))
                        // {
                        //     loader.sourceComponent = mainWindowPage
                        // }
                        // else
                        // {
                        //     systemErrorWindow.show();
                        // }
                        loader.sourceComponent = mainWindowPage
                    }
                    onHoveredChanged: {
                        if (loginButtonArea.containsMouse)
                        {
                            loginButtonRectangle.color = "#26C2FA"
                            loginButtonText.color = "white"
                        }
                        else
                        {
                            loginButtonRectangle.color = "white"
                            loginButtonText.color = "#26C2FA"
                        }
                    }
                }
            }
            Rectangle
            {
                id: registerButtonRectangle
                width: parent.width * 0.565
                height: parent.height * 0.0785
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: loginButtonRectangle.bottom
                anchors.topMargin: loginWindow.height * 0.04
                color: "#26C2FA"

                Text {
                    id: registerButtonText
                    font.pixelSize: 12
                    anchors.centerIn: parent
                    text: qsTr("注册")
                    color: "white"
                }
                MouseArea
                {
                    id: registerButtonArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        loader.sourceComponent = registerPage
                    }
                    onHoveredChanged: {
                        if (!registerButtonArea.containsMouse)
                        {
                            registerButtonRectangle.color = "#26C2FA"
                            registerButtonText.color = "white"
                        }
                        else
                        {
                            registerButtonText.color = "black"
                        }
                    }
                }
            }

            Text
            {
                id: systemVersion
                text: qsTr("V0.0.2")
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 1
                anchors.right: parent.right
                anchors.rightMargin: 1
            }
        }
    }
    // 监听窗口关闭事件
    onClosing: {
        // 调用 Qt.quit() 来终止程序
        Qt.quit();
    }

    SystemErrorDialog
    {
        id: systemErrorWindow
    }
}
