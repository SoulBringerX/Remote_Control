import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtMultimedia
import "./Dialog/"
Window {
    id: loginWindow
    maximumWidth: Screen.desktopAvailableWidth * 0.565
    maximumHeight: Screen.desktopAvailableHeight * 0.655
    minimumWidth: Screen.desktopAvailableWidth * 0.565
    minimumHeight: Screen.desktopAvailableHeight * 0.655
    visible: true
    title: qsTr("欢迎使用RemoteControl")
    flags: Qt.platform.os === "linux" ? Qt.Window | Qt.WindowCloseButtonHint : Qt.FramelessWindowHint | Qt.Window | Qt.WindowCloseButtonHint

    property bool registerstatus: true
    property int dragX: 0
    property int dragY: 0
    property bool dragging: false
    property bool isplaying: false

    Item {
        id: loginPage
        anchors.fill: parent

        Image {
            id: windowBackground
            source: "qrc:/images/sunback.jpg"
            anchors.fill: parent
        }

        Rectangle{
            id: loginRectangle
            height: parent.height
            width: parent.width*0.35
            color: "#E4F6FD"
            opacity: 0.8

            Rectangle
            {
                id:backgroundMediaPlayer
                width: parent.width * 0.65
                height: 20
                anchors.verticalCenter: themeColorChangedRectangle.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.01
                color:"transparent"
                opacity: 0.8

                MediaPlayer {
                    id: mediaPlayer
                    source: "qrc:/music/TAK,Lily - Stellar Dream.mp3"
                    onDurationChanged: {
                        progress.sDuration = " / " + msecs2String(duration);
                    }
                    audioOutput: AudioOutput {
                        id: audioOut
                        volume: 0.4
                    }
                }

                Rectangle {
                    width: parent.width * 0.5
                    height: parent.height
                    color:"transparent"

                    Text {
                        id: songName
                        width: parent.width
                        height: parent.height
                        text: "TAK, Lily - Stellar Dream"

                        // 使用一个持续的滚动动画
                        SequentialAnimation {
                            loops: Animation.Infinite
                            running: true

                            // 从右侧开始滚动
                            NumberAnimation {
                                target: songName
                                property: "x"
                                from: parent.width
                                to: -songName.width
                                duration: 6000 // 设置滚动时间
                            }

                            // 在滚动完成后立即重置位置并开始下一个滚动
                            PauseAnimation {
                                duration: 0
                            }

                            // 立即重置位置，准备下一个循环
                            NumberAnimation {
                                target: songName
                                property: "x"
                                from: -songName.width
                                to: parent.width
                                duration: 0 // 立即重置
                            }
                        }
                    }
                }
                Rectangle{
                    id: playButton
                    width:parent.height
                    height:width
                    color:"transparent"
                    anchors.right: parent.right

                    Image
                    {
                        source:isplaying ? "qrc:/images/pause.svg":"qrc:/images/play.svg"
                        anchors.centerIn: parent
                        width: parent.width
                        height: parent.height
                        fillMode: Image.PreserveAspectFit
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            isplaying = !isplaying
                            if(isplaying == true)
                            {
                                console.log("开始播放"+mediaPlayer.audioOutput)
                                mediaPlayer.play();
                            }
                            else
                            {
                                console.log("暂停播放"+mediaPlayer.audioOutput)
                                mediaPlayer.pause();
                            }
                        }
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
                anchors.topMargin: 5
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
                        z:3
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
                width: parent.width*0.75
                height: parent.height*0.0575
                anchors.top: sidBar.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: loginWindow.height * 0.0875

                Image{
                    id: accountIcon
                    source: "qrc:/images/Account.svg"
                    width: parent.height*0.8
                    height: parent.height*0.8
                    anchors.left: parent.left
                    anchors.leftMargin: userAccountRectangle.width * 0.01
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle
                {
                    width: parent.width*0.85
                    height: parent.height
                    anchors.right: parent.right
                    anchors.rightMargin:3.5
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
                width: parent.width*0.75
                height: parent.height*0.0575
                anchors.top: userAccountRectangle.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: loginWindow.height * 0.1

                Image{
                    id: passwordIcon
                    source: "qrc:/images/password.svg"
                    width: parent.height*0.8
                    height: parent.height*0.8
                    anchors.left: parent.left
                    anchors.leftMargin: userPasswordRectangle.width * 0.01
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle
                {
                    width: parent.width*0.85
                    height: parent.height
                    anchors.right: parent.right
                    anchors.rightMargin:3.5

                    TextInput
                    {
                        id: userPasswordInput
                        width: parent.width
                        anchors.centerIn: parent
                        readOnly: false
                        font.pixelSize: 12
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
                width: userPasswordRectangle.width 
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
                    z: 2
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked:
                    {
                        //登录逻辑
                        // if(account.loginCheck(userAccountInput.text,userPasswordInput.text))
                        // {
                        //     if(Qt.platform.os === "windows")
                        //         loader.sourceComponent = mainWindowPage
                        //     else
                        //         loader.sourceComponent = mainWindowPage
                        // }
                        // else
                        // {
                        //     systemErrorDialog.show()
                        //     // 清空账户密码输入
                        //     userPasswordInput.clear()
                        // }
                        //打桩测试
                        if(Qt.platform.os === "linux")
                        {
                            console.log('该用户使用的是Linux')
                            loader.sourceComponent = mainWindowPage
                        }
                        else
                        {
                            console.log('该用户使用的是Windows')
                            loader.sourceComponent = rcrmanWindowPage
                        }
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
                width: userPasswordRectangle.width
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
                    z: 1
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
                text: qsTr("V0.0.4")
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 1
                anchors.right: parent.right
                anchors.rightMargin: 1
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
    MouseArea {
        anchors.fill: parent
        z: -2 // 设置此 MouseArea 的 z 值为较低，确保它在文本和图标下方
        onPressed: {
            loginWindow.dragX = mouseX
            loginWindow.dragY = mouseY
            loginWindow.dragging = true
        }
        onReleased: loginWindow.dragging = false
        onPositionChanged: {
            if (loginWindow.dragging) {
                loginWindow.x += mouseX - loginWindow.dragX
                loginWindow.y += mouseY - loginWindow.dragY
            }
        }
    }
    SystemErrorDialog{
        id:systemErrorDialog
    }
}
