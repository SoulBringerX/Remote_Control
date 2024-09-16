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
                    width: 850
                    height: 40
                    Rectangle {
                        id: deviceInformationRectangle
                        anchors.fill: parent
                        Image {
                            id: deviceIconImage
                            width: 16
                            height: 16
                            source: "qrc:/images/Computer.svg"
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                        }
                        Text {
                            id: deviceNameText
                            anchors.left: deviceIconImage.right
                            anchors.leftMargin: 10
                            text: qsTr(deviceName)
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Image {
                            id: deviceConnectionStatusImage
                            width: 16
                            height: 16
                            source: model.isConnected ? "qrc:/images/Connected.svg" : "qrc:/images/Disconnected.svg"
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 20
                        }
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
