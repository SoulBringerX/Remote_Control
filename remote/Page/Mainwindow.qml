import QtQuick
import QtQuick.Controls
import "../Model" // 进入 Model 文件夹

Window {
    id: root
    maximumWidth: Screen.desktopAvailableWidth * 0.675
    maximumHeight: Screen.desktopAvailableHeight * 0.625
    minimumWidth: Screen.desktopAvailableWidth * 0.675
    minimumHeight: Screen.desktopAvailableHeight * 0.625
    visible: true
    title: qsTr("欢迎使用RemoteControl")

    Rectangle
    {
        id: userSideBar
        width: root.width * 0.15
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        color: "lightblue"

        Rectangle {
            id: userSideBarbutton
            width: 36
            height: 36
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
                        iconchanged = false
                    }
                    else
                    {
                        userSideBarExtendAnimation.running = true
                        userDeviceHideAnimation.running = true
                        backIconchanged2.running = true
                        directConnectionTextsee.running = true
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
        }

        Rectangle{
            id: directConnectionButtonRectangle
            width: parent.width - 36
            height: 36
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.top: userSideBarbutton.bottom
            anchors.topMargin: 50
            color: "transparent"

            Rectangle {
                id: directConnectionRectangle
                width: 36
                height: 36
                anchors.left: parent.left
                anchors.top: parent.top
                color: "transparent"

                Image {
                    id: directConnectionIcon
                    source: "qrc:/images/Directconnection2.svg"
                }
            }
            Text
            {
                id: directConnectionText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: directConnectionRectangle.right
                anchors.leftMargin: 10
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
                    directConnectionIcon.source = "qrc:/images/Directconnection1.svg"
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
    }

    Rectangle
    {
        id: userDevices
        width: root.width * 0.85
        height: parent.height
        anchors.left: userSideBar.right
        anchors.top: parent.top

        ListModel {
            id: deviceInformationModel
            ListElement { deviceName: "Computer 1"; isConnected: false }
            ListElement { deviceName: "Computer 2"; isConnected: false }
            ListElement { deviceName: "Computer 3"; isConnected: false }
        }

        Rectangle {
            id: userDeviceStatus
            width: parent.width * 0.625
            height: parent.height
            anchors.left: parent.left
            anchors.top: parent.top

            ListView{
                id: userDeviceList
                width: parent.width
                height: parent.height
                model: deviceInformationModel

                //数据格式
                delegate: Item {
                    id: deviceListItem
                    width: parent.width
                    height: 40
                    Rectangle {
                        id: deviceInformationRectangle
                        anchors.fill: parent
                        border.color: "black"
                        border.width: 0.5
                        Image {
                            id: deviceIconImage
                            width: 16
                            height: 16
                            source: "qrc:/images/Computer.svg"
                            anchors.top: parent.top
                            anchors.topMargin: 10
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                        }
                        Text {
                            id: deviceNameText
                            anchors.left: deviceIconImage.right
                            anchors.leftMargin: 10
                            text: qsTr(deviceName)
                             anchors.top: parent.top
                             anchors.topMargin: 10
                        }
                        Rectangle
                        {
                            id: deviceExtendStatusRectangle
                            width: 16
                            height: 16
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.topMargin: 10
                            anchors.rightMargin: 20
                            Image {
                                id: deviceExtendStatusImage
                                width: 16
                                height: 16
                                anchors.top: parent.top
                                source: "qrc:/images/extend.svg"
                                rotation: 90
                            }
                            MouseArea
                            {
                                id: userDeviceInformationExtendMouseArea
                                property bool iconchanged: false;
                                anchors.fill: parent
                                onClicked: {
                                    if(!iconchanged){
                                        userDeviceInformationExtend.running = true
                                        directionIconchanged1.running = true
                                        iconchanged = true
                                    }
                                    else
                                    {
                                        userDeviceInformationShrink.running = true
                                        directionIconchanged2.running = true
                                        iconchanged = false
                                    }
                                }
                            }
                            PropertyAnimation {
                                id: directionIconchanged1
                                properties: "rotation"
                                target: deviceExtendStatusImage
                                from: 90
                                to: 0
                                duration: 250 // 动画持续时间，以毫秒为单位
                            }
                            PropertyAnimation {
                                id:  directionIconchanged2
                                properties: "rotation"
                                target: deviceExtendStatusImage
                                from: 0
                                to: 90
                                duration: 250 // 动画持续时间，以毫秒为单位
                            }
                        }

                        Rectangle {
                            id: userDeviceDetailsInformationRectangle
                            width: parent.width - 21
                            height: 60
                            anchors.top: deviceExtendStatusRectangle.bottom
                            anchors.topMargin: 5
                            anchors.right: deviceExtendStatusRectangle.right
                            visible: userDeviceInformationExtendMouseArea.iconchanged

                            Flickable {
                                id: flickable
                                width: parent.width
                                height: parent.width
                                contentWidth: parent.width
                                contentHeight: 160

                                Rectangle {
                                    id: userDeviceAppListRectangle
                                    width: flickable.width
                                    height: 160

                                    //测试专用
                                    ListModel {
                                        id: deviceAppListModel
                                        ListElement {
                                            deviceIP: "192.168.1.1"
                                            deviceName: "Computer 1"
                                            AppName: "App 1"
                                            RdpAppName: "App 1"
                                            AppIconPath: "qrc:/images/wx.svg"
                                            isConnected: true
                                        }
                                        ListElement {
                                            deviceIP: "192.168.1.1"
                                            deviceName: "Computer 1"
                                            AppName: "App 2"
                                            RdpAppName: "App 2"
                                            AppIconPath: "qrc:/images/QQ.svg"
                                            isConnected: true
                                        }
                                    }

                                    ListView {
                                        id: deviceAppListView
                                        anchors.fill: parent
                                        model: deviceAppListModel

                                        delegate: Item {
                                            id: appItem
                                            width: parent.width
                                            height: 32
                                            Rectangle {
                                                id: appItemRectangle
                                                anchors.fill: parent

                                                Image{
                                                    id: appIcon
                                                    width: 24
                                                    height: 24
                                                    source: model.AppIconPath
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    anchors.left: parent.left
                                                    anchors.leftMargin: 20
                                                }

                                                Text{
                                                    id: appName
                                                    text: model.AppName
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    anchors.left: appIcon.right
                                                    anchors.rightMargin: 10
                                                }
                                            }
                                        }
                                    }
                                }

                                // 启用滚动条
                                flickableDirection: Flickable.VerticalFlick // 垂直滚动
                            }
                        }
                    }
                    PropertyAnimation
                    {
                        id: userDeviceInformationExtend
                        target: deviceListItem
                        properties: "height"
                        from: 40
                        to: userDeviceList.height / userDeviceList.count
                        duration: 250
                    }
                    PropertyAnimation
                    {
                        id: userDeviceInformationShrink
                        target: deviceListItem
                        properties: "height"
                        from: userDeviceList.height /  userDeviceList.count
                        to: 40
                        duration: 250
                    }
                }
            }
        }

        Rectangle {
            id: userDeviceInformationRectangle
            width: parent.width * 0.475
            height: parent.width
            anchors.left: userDeviceStatus.right
            anchors.top: parent.top
            color: "#EAF1F3"
        }
    }

    // 界面框架动画组
    PropertyAnimation {
        id: userSideBarHideAnimation
        properties: "width"
        target: userSideBar
        from: root.width * 0.15
        to: 64
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userDeviceExtendAnimation
        properties: "width"
        target: userDevices
        from: root.width * 0.85
        to: root.width - 64
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userSideBarExtendAnimation
        properties: "width"
        target: userSideBar
        from: 64
        to: root.width * 0.15
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userDeviceHideAnimation
        properties: "width"
        target: userDevices
        from: root.width - 64
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

    // 监听窗口关闭事件
    onClosing: {
        // 调用 Qt.quit() 来终止程序
        Qt.quit();
    }

}
