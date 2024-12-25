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
                            windowBackground.source = "qrc:/images/back1.jpg"
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
                    console.log("打开资源管理器")
                    account.openFileManager()
                }
            }
        }
        Rectangle
        {
            id: userAccountRectangle
            width: parent.width
            height: parent.height * 0.05
            anchors.top: sidBar.bottom
            anchors.topMargin: parent.height * 0.1
            color: "transparent"

            Text{
                id: accountText
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.09
                color: "white"
                text: qsTr("帐号：")
            }

            Rectangle
            {
                width: parent.width * 0.7
                height: parent.height
                border.color: "black"
                border.width: 0.5
                anchors.left: accountText.right
                anchors.leftMargin: parent.width * 0.03
                color: "white"

                TextInput
                {
                    id: userAccountInput
                    width: parent.width
                    anchors.centerIn: parent
                    readOnly: false
                    font.pixelSize: 12
                    selectionColor: "lightblue" // 选中文本的背景色
                    selectedTextColor: "white" // 选中文本的颜色
                    Keys.onEnterPressed: userAccountInput.focus = false // 按下Enter键时取消焦点
                    Keys.onReturnPressed: userAccountInput.focus = false // 按下Return键时取消焦点
                    clip: true //设置组件中文字不得超出长度
                }
            }
        }
        Rectangle
        {
            id: userPasswordRectangle
            width: parent.width
            height: parent.height * 0.05
            anchors.top: userAccountRectangle.bottom
            anchors.topMargin: parent.height * 0.1
            color: "transparent"

            Text{
                id: passwordText
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.09
                color: "white"
                text: qsTr("密码：")
            }

            Rectangle
            {
                width: parent.width * 0.7
                height: parent.height
                border.color: "black"
                border.width: 0.5
                anchors.left: passwordText.right
                anchors.leftMargin: parent.width * 0.03

                TextInput
                {
                    id: userPasswordInput
                    width: parent.width
                    anchors.centerIn: parent
                    readOnly: false
                    font.pixelSize: 12
                    selectionColor: "lightblue" // 选中文本的背景色
                    selectedTextColor: "white" // 选中文本的颜色
                    Keys.onEnterPressed: userAccountInput.focus = false // 按下Enter键时取消焦点
                    Keys.onReturnPressed: userAccountInput.focus = false // 按下Return键时取消焦点
                    clip: true //设置组件中文字不得超出长度
                    echoMode: TextInput.Password
                }
            }
        }
        Rectangle
        {
            id: registerButtonRectangle
            width: root.width * 0.6
            height: root.height * 0.0875
            anchors.top: userPasswordRectangle.bottom
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter

            Text{
                id:registerButtonText
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("确认注册")
            }

            MouseArea
            {
                id: registerButtonArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked:{
                    // if(account.registerCheck(userAccountInput.text,userPasswordInput.text))
                    // {
                    //     registerDialog.show()
                    // }
                    // else
                    //     errorDialog.show()
                    // 打桩测试
                    registerDialog.show()
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
    }
}
