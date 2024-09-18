import QtQuick
import QtQuick.Controls
import "../Model" // 进入 Model 文件夹

Window {
    id: root
    width: 1280
    height: 720
    visible: true
    title: qsTr("欢迎使用RemoteControl")

    Rectangle
    {
        id: userSideBar
        width: 150
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        color: "lightblue"

        //侧边栏按钮收缩
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
                onClicked: {
                    //侧边栏动画收缩
                    if(iconchanged)
                    {
                        userSideBarHideAnimation.running = true
                        userDeviceExtendAnimation.running = true
                        backIconchanged1.running = true
                        iconchanged = false
                    }
                    else
                    {
                        userSideBarExtendAnimation.running = true
                        userDeviceHideAnimation.running = true
                        backIconchanged2.running = true
                        iconchanged = true
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

    Rectangle
    {
        id: userDevices
        width: 1130
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
            width: 850
            height: 720
            anchors.left: parent.left
            anchors.top: parent.top

            ListView{
                id: userDeviceList
                width: 850
                height: 720
                model: deviceInformationModel

                //数据格式
                delegate: Item {
                    id: deviceListItem
                    width: 850
                    height: 40
                    Rectangle {
                        id: deviceInformationRectangle
                        anchors.fill: parent
                        border.color: "black"
                        border.width: 1
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
                            }
                            MouseArea
                            {
                                id: userDeviceInformationExtendMouseArea
                                property bool iconchanged: false;
                                anchors.fill: parent
                                onClicked: {
                                    if(!iconchanged){
                                        userDeviceInformationExtend.running = true
                                        iconchanged = true
                                    }
                                    else
                                    {
                                        userDeviceInformationShrink.running = true
                                        iconchanged = false
                                    }
                                }
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
            width: parent.width -  userDeviceStatus.width
            height: parent.width
            anchors.left: userDeviceStatus.right
            anchors.top: parent.top
            color: "lightgrey"
        }
    }


    PropertyAnimation {
        id: userSideBarHideAnimation
        properties: "width"
        target: userSideBar
        from: 150
        to: 64
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userDeviceExtendAnimation
        properties: "width"
        target: userDevices
        from: 1130
        to: 1216
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userSideBarExtendAnimation
        properties: "width"
        target: userSideBar
        from: 64
        to: 150
        duration: 500 // 动画持续时间，以毫秒为单位
    }
    PropertyAnimation {
        id: userDeviceHideAnimation
        properties: "width"
        target: userDevices
        from: 1216
        to: 1130
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
}
