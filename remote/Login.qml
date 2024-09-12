import QtQuick

Window {
    id: loginWindow
    width: 426
    height: 440
    visible: true
    title: qsTr("欢迎使用RemoteControl")
    Item {
        anchors.fill: parent // 填充整个窗口
        Image {
            source: GlobalProperties.preview ? "./images/back1.jpg" : ":/images/back1.jpg "
            fillMode: Image.PreserveAspectFit
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
                border.width: 1
                border.color: "black"
                radius: width / 2
                x: 0

                Image
                {
                    source: GlobalProperties.preview ? "./images/sun.svg" : ":/images/sun.svg"
                    fillMode: Image.PreserveAspectFit
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
                        }
                        else
                        {
                            themeColorChangedAnimation2.running = true
                            iconchanged = true
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
        }
        Rectangle
        {
            id: userAccountRectangle
            width: 340
            height: 20
            anchors.top: sidBar.bottom
            anchors.left: loginWindow.left
            anchors.topMargin: 65

            Text{
                id: accountText
                anchors.left: parent.left
                anchors.leftMargin: 30
                text: qsTr("帐号：")
            }

            Rectangle
            {
                width: 310
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
                    color: "black"
                    readOnly: false
                }
            }
        }
        Rectangle
        {
            id: userPasswordRectangle
            width: 340
            height: 20
            anchors.top: userAccountRectangle.bottom
            anchors.left: loginWindow.left
            anchors.topMargin: 65

            Text{
                id: passwordText
                anchors.left: parent.left
                anchors.leftMargin: 30
                text: qsTr("密码：")
            }

            Rectangle
            {
                width: 310
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
                    color: "black"
                    readOnly: false
                    echoMode: TextInput.Password
                }
            }
        }
    }
}
