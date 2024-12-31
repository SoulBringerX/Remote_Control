import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "../Model" // 进入 Model 文件夹
Rectangle{
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    ListModel {
        id: deviceInformationModel
        ListElement { deviceName: "Computer 1"; isConnected: true }
        ListElement { deviceName: "Computer 2"; isConnected: false }
        ListElement { deviceName: "Computer 3"; isConnected: false }
    }
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
                        color:"transparent"
                        z:3
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
                        id: userDeviceInformationIcon
                        width: 16
                        height: 16
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: deviceExtendStatusRectangle.right
                        anchors.rightMargin: parent.width * 0.05
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        Image{
                            width: 16
                            height: 16
                            anchors.top: parent.top
                            source: "qrc:/images/info.svg"
                        }
                        MouseArea{
                            id: userDeviceItemInformation
                            anchors.fill: parent
                            onClicked: {
                                userDeviceInformation.text = "OS:Windosw 10" + '\n'+"CPU:Intel i5-8500"
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
                            height: parent.height
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
