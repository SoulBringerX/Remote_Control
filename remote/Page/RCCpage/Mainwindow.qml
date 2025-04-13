import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Controls.Basic

import './'
import '../RCRpage/'

Window {
    id: root
    maximumWidth: Screen.desktopAvailableWidth * 0.625
    maximumHeight: Screen.desktopAvailableHeight * 0.675
    minimumWidth: Screen.desktopAvailableWidth * 0.625
    minimumHeight: Screen.desktopAvailableHeight * 0.675
    visible: true
    title: qsTr("欢迎使用RemoteControl")
    flags: Qt.platform.os === "linux" ? Qt.Window | Qt.WindowCloseButtonHint : Qt.FramelessWindowHint | Qt.Window | Qt.WindowCloseButtonHint

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
                        personalSettingTextmiss.running = true
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
                        personalSettingTextsee.running = true
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
            PropertyAnimation
            {
                id: personalSettingTextmiss
                target: personalSettingText
                properties: "opacity"
                from: 1
                to: 0
                duration: 500
            }
            PropertyAnimation
            {
                id: personalSettingTextsee
                target: personalSettingText
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
                    personalSettingIcon.source = "qrc:/images/personalSetting1.svg";
                    personalSettingText.color = "black";
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
                    personalSettingIcon.source = "qrc:/images/personalSetting1.svg";
                    personalSettingText.color = "black";
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
                    rdpConnectionSettingIcon.source = "qrc:/images/configureSetting2.svg";
                    rdpConnectionSettingText.color = "black";
                    personalSettingIcon.source = "qrc:/images/personalSetting1.svg";
                    personalSettingText.color = "black";
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
        Rectangle{
            id: personalSettingButtonRectangle
            width: parent.width*0.75
            height: 32
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: rdpDeviceButtonRectangle.bottom
            anchors.topMargin: parent.height * 0.075
            color: "transparent"
            radius: 5

            Rectangle {
                id: personalSettingRectangle
                width: 32
                height: 32
                anchors.left: parent.left
                anchors.top: parent.top
                color: "transparent"

                Image {
                    width: 32
                    height: 32
                    id: personalSettingIcon
                    source: "qrc:/images/personalSetting1.svg"
                    fillMode: Image.PreserveAspectFit
                }
            }
            Text
            {
                id: personalSettingText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: personalSettingRectangle.right
                anchors.leftMargin: 5
                text: qsTr("个人中心")
                opacity: 1.0
            }
            MouseArea
            {
                id: personalSettingArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked:
                {
                    personalSettingButtonRectangle.color = "#26C2FA";
                    personalSettingText.color = "#1296db";
                    personalSettingIcon.source = "qrc:/images/personalSetting2.svg";
                    directConnectionIcon.source = "qrc:/images/remoteConnecting2.svg";
                    directConnectionText.color = "black";
                    rdpConnectionSettingIcon.source = "qrc:/images/configureSetting2.svg";
                    rdpConnectionSettingText.color = "black";
                    rdpDeviceIcon.source = "qrc:/images/Device1.svg";
                    rdpDeviceText.color = "black";
                    pageloader.sourceComponent = personalSettingPage;
                }
                onHoveredChanged:
                {
                    if (personalSettingArea.containsMouse)
                    {
                        personalSettingButtonRectangle.color = "#9EE6FF"
                    }
                    else
                    {
                        personalSettingButtonRectangle.color = "transparent"
                    }
                }
            }
        }
    }

    Connections {
        target: deviceInfoManager
        onDeviceInfoChanged: {
            userDeviceInformation.text =
                "🖥️ CPU: " + deviceInfoManager.deviceInfo.cpuModel + "\n" +
                "🧩 Cores: " + deviceInfoManager.deviceInfo.cpuCores + "\n" +
                "⚡ Usage: " + deviceInfoManager.deviceInfo.cpuUsage + "%\n" +
                "🌡️ CPU Temp: " + deviceInfoManager.deviceInfo.cpuTemperature + "°C\n" +
                "💾 Memory: " + Math.floor(deviceInfoManager.deviceInfo.usedMemory / (1024 * 1024)) + "MB / " + Math.floor(deviceInfoManager.deviceInfo.totalMemory / (1024 * 1024)) + "MB\n" +
                "🗄️ Disk: " + Math.floor(deviceInfoManager.deviceInfo.usedDisk / (1024 * 1024 * 1024)) + "GB / " + Math.floor(deviceInfoManager.deviceInfo.totalDisk / (1024 * 1024 * 1024)) + "GB\n" +
                "🎮 GPU: " + deviceInfoManager.deviceInfo.gpuModel + "\n" +
                "🌡️ GPU Temp: " + deviceInfoManager.deviceInfo.gpuTemperature + "°C";
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
            color: "#F0EAE6"
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
            Component {
                id: personalSettingPage
                PersonalSetting{}
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
            Text {
                id: userDeviceInformation
                text: "设备信息加载中..."
                font.pixelSize: 14
                font.bold: true
                wrapMode: Text.WordWrap
                color: "#333333"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                width: parent.width * 0.9
                anchors.top: lastLogininformation.bottom
                anchors.topMargin: 15
                anchors.horizontalCenter: parent.horizontalCenter
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
        drag.target: root  // 指定拖拽目标为窗口本身
        drag.axis: Drag.XAxis | Drag.YAxis  // 允许在 X 和 Y 轴上拖拽
        z:-1
        // 仅在 Windows 系统上启用拖拽功能
        enabled: Qt.platform.os === "windows"

        onPressed: {
            root.dragX = mouseX
            root.dragY = mouseY
            root.dragging = true
        }
        onReleased: {
            root.dragging = false
        }
        onPositionChanged: {
            if (root.dragging) {
                root.x += mouseX - root.dragX
                root.y += mouseY - root.dragY
                root.dragX = mouseX
                root.dragY = mouseY
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
    // 监听窗口关闭事件
    onClosing: {
        // 调用 Qt.quit() 来终止程序
        Qt.quit();
    }
}
