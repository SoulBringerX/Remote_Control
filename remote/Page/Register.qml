import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: root
    maximumWidth: 426
    maximumHeight: 440
    minimumWidth: 426
    minimumHeight: 440
    visible: true
    title: qsTr("欢迎使用RemoteControl")
    Item {
        id: registerPage
        anchors.fill: parent // 填充整个窗口
        Image {
            id: windowBackground
            source: "qrc:/images/sunback.jpg"
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
            anchors.topMargin: 65
            anchors.horizontalCenter: parent.horizontalCenter
            color: "lightblue"
            radius: height / 2
        }
        Rectangle
        {
            id: userAccountRectangle
            width: 320
            height: 24
            anchors.top: sidBar.bottom
            anchors.left: root.left
            anchors.leftMargin: 90
            anchors.topMargin: 65
            color: "transparent"

            Text{
                id: accountText
                anchors.left: parent.left
                anchors.leftMargin: 60
                color: "white"
                text: qsTr("帐号：")
            }

            Rectangle
            {
                width: 220
                height: parent.height
                border.color: "black"
                border.width: 1
                anchors.left: accountText.right
                anchors.leftMargin: 20

                TextInput
                {
                    id: userAccountInput
                    width: parent.width
                    height: parent.heigh
                    readOnly: false
                    font.pixelSize: 18
                }
            }
        }
        Rectangle
        {
            id: userPasswordRectangle
            width: 320
            height: 24
            anchors.top: userAccountRectangle.bottom
            anchors.left: root.left
            anchors.leftMargin: 90
            anchors.topMargin: 65
            color: "transparent"

            Text{
                id: passwordText
                anchors.left: parent.left
                anchors.leftMargin: 60
                color: "white"
                text: qsTr("密码：")
            }

            Rectangle
            {
                width: 220
                height: parent.height
                border.color: "black"
                border.width: 1
                anchors.left: passwordText.right
                anchors.leftMargin: 20

                TextInput
                {
                    id: userPasswordInput
                    width: parent.width
                    height: parent.heigh
                    readOnly: false
                    font.pixelSize: 18
                    echoMode: TextInput.Password
                }
            }
        }
        Rectangle
        {
            id: registerButtonRectangle
            width: 80
            height: 25
            anchors.top: userPasswordRectangle.bottom
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter

            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("确认注册")
            }

            MouseArea
            {
                anchors.fill: parent
                onClicked:{
                    registerDialog.show()
                }
            }
        }
    }
    // 监听窗口关闭事件
    onClosing: {
        // 调用 Qt.quit() 来终止程序
        Qt.quit();
    }

    RegisterDialog
    {
        id: registerDialog
        visible: false
    }
}