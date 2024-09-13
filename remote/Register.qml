import QtQuick

Window {
    id: root
    width: 426
    height: 440
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
            anchors.left: loginWindow.left
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
            anchors.left: loginWindow.left
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
    }
}
