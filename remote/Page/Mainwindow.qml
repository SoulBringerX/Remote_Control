import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Controls.Basic

Window {
    id: root
    maximumWidth: Screen.desktopAvailableWidth * 0.625
    maximumHeight: Screen.desktopAvailableHeight * 0.675
    minimumWidth: Screen.desktopAvailableWidth * 0.625
    minimumHeight: Screen.desktopAvailableHeight * 0.675
    visible: true
    title: qsTr("欢迎使用RemoteControl")
    flags: Qt.FramelessWindowHint | Qt.Window | Qt.WindowCloseButtonHint

    property bool registerstatus: true
    property int dragX: 0
    property int dragY: 0
    property bool dragging: false
    property bool isplaying: false

    signal minimizeToTray()

    //用户栏
    Rectangle
    {
        id: userSideBar
        width: root.width * 0.15
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        color: "white"

        Rectangle {
            id: userSideBarbutton
            width: 28
            height: 28
            radius: height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 10
            color: "transparent"

            Image {
                id: userSideBarHideButton
                rotation: 0
                source: "qrc:/images/back1.svg"
            }

            MouseArea {
                property bool iconchanged: true;
                anchors.fill: parent
                onClicked:
                {
                    //侧边栏动画收缩
                    if(iconchanged)
                    {
                        userSideBarHideAnimation.running = true
                        userDeviceExtendAnimation.running = true
                        backIconchanged1.running = true
                        directConnectionTextmiss.running = true
                        rdpConnectionSettingTextmiss.running = true
                        rdpDeviceTextmiss.running = true
                        iconchanged = false
                    }
                    else
                    {
                        userSideBarExtendAnimation.running = true
                        userDeviceHideAnimation.running = true
                        backIconchanged2.running = true
                        directConnectionTextsee.running = true
                        rdpConnectionSettingTextsee.running = true
                        rdpDeviceTextsee.running = true
                        iconchanged = true
                    }
                }
            }
            PropertyAnimation
            {
                id: directConnectionTextmiss
                target: directConnectionText
                properties: "opacity"
                from: 1
                to: 0
                duration: 500
            }
            PropertyAnimation
            {
                id: directConnectionTextsee
                target: directConnectionText
                properties: "opacity"
                from: 0
                to: 1
                duration: 500
            }
            PropertyAnimation
            {
                id: rdpConnectionSettingTextmiss
                target: rdpConnectionSettingText
                properties: "opacity"
                from: 1
                to: 0
                duration: 500
            }
            PropertyAnimation
            {
                id: rdpConnectionSettingTextsee
                target: rdpConnectionSettingText
                properties: "opacity"
                from: 0
                to: 1
                duration: 500
            }
            PropertyAnimation
            {
                id: rdpDeviceTextmiss
                target: rdpDeviceText
                properties: "opacity"
                from: 1
                to: 0
                duration: 500
            }
            PropertyAnimation
            {
                id: rdpDeviceTextsee
                target: rdpDeviceText
                properties: "opacity"
                from: 0
                to: 1
                duration: 500
            }
        }

        Rectangle{
            id: directConnectionButtonRectangle
            width: parent.width*0.75
            height: 32
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: userSideBarbutton.bottom
            anchors.topMargin: parent.height * 0.075
            color: "transparent"
            radius: 5

            Rectangle {
                id: directConnectionRectangle
                width: 32
                height: 32
                anchors.left: parent.left
                anchors.top: parent.top
                color: "transparent"
                Image {
                    width: 32
                    height: 32
                    id: directConnectionIcon
                    source: "qrc:/images/remoteConnecting2.svg"
                    fillMode: Image.PreserveAspectFit
                }
            }
            Text
            {
                id: directConnectionText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: directConnectionRectangle.right
                anchors.leftMargin: 5
                text: qsTr("远程控制")
                opacity: 1.0
            }
            MouseArea
            {
                id: directConnectionArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked:
                {
                    directConnectionButtonRectangle.color = "#26C2FA";
                    directConnectionText.color = "#1296db"
                    directConnectionIcon.source = "qrc:/images/remoteConnecting1.svg";
                    rdpConnectionSettingIcon.source = "qrc:/images/configureSetting2.svg";
                    rdpDeviceIcon.source = "qrc:/images/Device1.svg";
                    rdpDeviceText.color = "black";
                    rdpConnectionSettingText.color = "black";
                    pageloader.sourceComponent = directConnectPage;
                }
                onHoveredChanged:
                {
                    if (directConnectionArea.containsMouse)
                    {
                        directConnectionButtonRectangle.color = "#9EE6FF"
                    }
                    else
                    {
                        directConnectionButtonRectangle.color = "transparent"
                    }
                }
            }
        }

        Rectangle{
            id: rdpConnectionSettingButtonRectangle
            width: parent.width*0.75
            height: 32
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: directConnectionButtonRectangle.bottom
            anchors.topMargin: parent.height * 0.075
            color: "transparent"
            radius: 5
            Rectangle {
                id: rdpConnectionSettingRectangle
                width: 32
                height: 32
                anchors.left: parent.left
                anchors.top: parent.top
                color: "transparent"

                Image {
                    width: 32
                    height: 32
                    id: rdpConnectionSettingIcon
                    source: "qrc:/images/configureSetting2.svg"
                    fillMode: Image.PreserveAspectFit
                }
            }
            Text
            {
                id: rdpConnectionSettingText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: rdpConnectionSettingRectangle.right
                anchors.leftMargin: 5
                text: qsTr("连接配置")
                opacity: 1.0
            }
            MouseArea
            {
                id: rdpConnectionSettingArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked:
                {
                    rdpConnectionSettingButtonRectangle.color = "#26C2FA";
                    rdpConnectionSettingText.color = "#1296db";
                    rdpConnectionSettingIcon.source = "qrc:/images/configureSetting1.svg";
                    directConnectionIcon.source = "qrc:/images/remoteConnecting2.svg";
                    directConnectionText.color = "black";
                    rdpDeviceIcon.source = "qrc:/images/Device1.svg";
                    rdpDeviceText.color = "black";
                    pageloader.sourceComponent = remoteSettingPage
                }
                onHoveredChanged:
                {
                    if (rdpConnectionSettingArea.containsMouse)
                    {
                        rdpConnectionSettingButtonRectangle.color = "#9EE6FF"
                    }
                    else
                    {
                        rdpConnectionSettingButtonRectangle.color = "transparent"
                    }
                }
            }
        }

        Rectangle{
            id: rdpDeviceButtonRectangle
            width: parent.width*0.75 
            height: 32
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: rdpConnectionSettingButtonRectangle.bottom
            anchors.topMargin: parent.height * 0.075
            color: "transparent"
            radius: 5
            Rectangle {
                id: rdpDeviceRectangle
                width: 32
                height: 32
                anchors.left: parent.left
                anchors.top: parent.top
                color: "transparent"

                Image {
                    width: 32
                    height: 32
                    id: rdpDeviceIcon
                    source: "qrc:/images/Device1.svg"
                    fillMode: Image.PreserveAspectFit
                }
            }
            Text
            {
                id: rdpDeviceText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: rdpDeviceRectangle.right
                anchors.leftMargin: 5
                text: qsTr("设备管理")
                opacity: 1.0
            }
            MouseArea
            {
                id: rdpDeviceArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked:
                {
                    rdpDeviceButtonRectangle.color = "#26C2FA";
                    rdpDeviceText.color = "#1296db";
                    rdpDeviceIcon.source = "qrc:/images/Device2.svg";
                    directConnectionIcon.source = "qrc:/images/remoteConnecting2.svg";
                    directConnectionText.color = "black";
                    pageloader.sourceComponent = userDevicesPage;
                }
                onHoveredChanged:
                {
                    if (rdpDeviceArea.containsMouse)
                    {
                        rdpDeviceButtonRectangle.color = "#9EE6FF"
                    }
                    else
                    {
                        rdpDeviceButtonRectangle.color = "transparent"
                    }
                }
            }
        }
    }

    Rectangle
    {
        id: userDevices
        width: root.width * 0.85
        height: parent.height
        anchors.left: userSideBar.right
        anchors.top: parent.top
        Rectangle {
            id: userDeviceStatus
            width: parent.width * 0.625
            height: parent.height
            anchors.left: parent.left
            anchors.top: parent.top
            z: 2
            color: "#4281ff"
            Loader{
                id: pageloader
                anchors.fill: parent
                // 初始加载主页面
                sourceComponent: directConnectPage
            }
            Component {
                id: userDevicesPage
                UserDevices{}
            }
            Component {
                id: remoteSettingPage
                RemoteSetting{}
            }
            Component {
                id: directConnectPage
                DirectConnect{}
            }
        }
        // 用户远程设备的信息（需登录过一次设备后方可显示对面的设备信息）
        Rectangle {
            id: userDeviceInformationRectangle
            width: parent.width * 0.375
            height: parent.height
            anchors.left: userDeviceStatus.right
            anchors.top: parent.top
            color: "#EAF1F3"

            Text{
                id: lastLogininformation
                text:"2024/12/30 10:30 am"
                anchors.top: parent.top
                anchors.topMargin: parent.height * 0.05
                anchors.horizontalCenter: userDeviceInformationRectangle.horizontalCenter
            }
            Timer {
                interval: 1000
                repeat: true
                running: true
                onTriggered: {
                    lastLogininformation.text = new Date().toLocaleString(Qt.locale("zh_CN"), "yyyy/MM/dd hh:mm ap")
                }
            }
            Text{
                id:userDeviceInformation
                text:" "
                anchors.top: lastLogininformation.bottom
                anchors.topMargin: parent.height * 0.05
                anchors.horizontalCenter: userDeviceInformationRectangle.horizontalCenter
            }
        }
    }

    // 界面框架动画组
    PropertyAnimation {
        id: userSideBarHideAnimation
        properties: "width"
        target: userSideBar
        from: root.width * 0.15
        to: 48
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userDeviceExtendAnimation
        properties: "width"
        target: userDevices
        from: root.width * 0.85
        to: root.width - 48
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userSideBarExtendAnimation
        properties: "width"
        target: userSideBar
        from: 48
        to: root.width * 0.15
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userDeviceHideAnimation
        properties: "width"
        target: userDevices
        from: root.width - 48
        to: root.width * 0.85
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: backIconchanged1
        properties: "rotation"
        target: userSideBarHideButton
        from: 0
        to: 180
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: backIconchanged2
        properties: "rotation"
        target: userSideBarHideButton
        from: 180
        to: 0
        duration: 500 // 动画持续时间，以毫秒为单位
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
                closeDialog.open()
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

    MouseArea {
        anchors.fill: parent
        z: -2 // 设置此 MouseArea 的 z 值为较低，确保它在文本和图标下方
        onPressed: {
            root.dragX = mouseX
            root.dragY = mouseY
            root.dragging = true
        }
        onReleased: root.dragging = false
        onPositionChanged: {
            if (root.dragging) {
                root.x += mouseX - root.dragX
                root.y += mouseY - root.dragY
            }
        }
    }
            // 弹出对话框
    Dialog {
    id: closeDialog
    title: "退出程序"
    modal: true
    width: 400
    height: 150
    standardButtons: Dialog.NoButton

    // 将对话框居中显示
    Component.onCompleted: {
        closeDialog.anchors.centerIn = parent
    }

    contentItem: Column {
        spacing: 15
        anchors.fill: parent
        anchors.margins: 20

        Text {
            id: closeDialogText
            text: "您想要关闭程序还是最小化到托盘？"
            wrapMode: Text.WordWrap
            font.pixelSize: 16
            color: "#333333"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.2
        }

        Row {
            spacing: 10
            anchors.top: closeDialogText.bottom
            anchors.topMargin: parent.height * 0.125
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "关闭程序"
                width: 100
                height: 40
                font.pixelSize: 14
                background: Rectangle{
                    color: "#d9534f"
                }
                onClicked: {
                    Qt.quit() // 关闭程序
                }
            }

            Button {
                text: "最小化到托盘"
                width: 100
                height: 40
                font.pixelSize: 14
                background: Rectangle{
                    color: "#5bc0de"
                }
                onClicked: {
                    // 最小化到托盘关闭当前的窗口，并隐藏主窗口
                    root.close()
                    minimizeToTray()
                    closeDialog.close()
                }
            }

            Button {
                text: "取消"
                width: 100
                height: 40
                font.pixelSize: 14
                background: Rectangle{
                    color: "#f0ad4e"
                }
                onClicked: closeDialog.close()
            }
        }
    }
}
}
