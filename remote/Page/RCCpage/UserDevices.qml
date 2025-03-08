import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "../../Model" // 进入 Model 文件夹
import "./"

Rectangle {
    id: root
    color: "transparent"

    // 设备数据模型，增加了 extended 字段用于记录是否展开
    ListModel {
        id: deviceInformationModel
        ListElement { deviceName: "Computer 1"; deviceIP: "192.168.1.1"; account: "user1"; password: "pass1"; isConnected: true; extended: false }
        ListElement { deviceName: "Computer 2"; deviceIP: "192.168.1.2"; account: "user2"; password: "pass2"; isConnected: false; extended: false }
        ListElement { deviceName: "Computer 3"; deviceIP: "192.168.1.3"; account: "user3"; password: "pass3"; isConnected: false; extended: false }
    }
    property int currentIndex: 0

    // 计算当前展开的项数
    function calcExpandedCount() {
        var cnt = 0;
        for (var i = 0; i < deviceInformationModel.count; i++) {
            if (deviceInformationModel.get(i).extended === true)
                cnt++;
        }
        return cnt;
    }

    ListView {
        id: userDeviceList
        width: parent.width
        height: parent.height
        model: deviceInformationModel

        delegate: Item {
            id: deviceListItem
            width: parent.width

            // 基本高度（未展开时高度）
            property int normalHeight: 40

            // 当该项展开时，其高度计算为：基本高度 + (剩余高度平均分给所有展开项)
            // 剩余高度 = ListView.height - (所有项的基本高度总和)
            // 如果当前没有展开项，则保持基本高度（避免除零）
            height: extended
                     ? normalHeight + ((userDeviceList.height - (deviceInformationModel.count * normalHeight)) / (calcExpandedCount() > 0 ? calcExpandedCount() : 1))
                     : normalHeight

            // 为高度变化添加动画
            Behavior on height {
                PropertyAnimation { duration: 250 }
            }

            // 注意：这里直接使用模型中传入的 extended、deviceName、deviceIP 等字段

            // 设备基本信息区域
            Rectangle {
                id: deviceInformationRectangle
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: normalHeight
                border.color: "black"
                border.width: 0.5
                color: "transparent"

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
                    MouseArea {
                        id: deviceNameMouseArea
                        anchors.fill: parent
                        onDoubleClicked: {
                            // 双击设备名称时，使用 FreeRDP 连接
                            console.log("Connecting to device:", deviceIP)
                            remoteControlThread.startConnection(deviceIP, account, password)
                            remoteView.show()
                        }
                    }
                }

                RemoteDraw {
                    id: remoteView
                }

                // 右上角展开/收缩图标
                Rectangle {
                    id: deviceExtendStatusRectangle
                    width: 16
                    height: 16
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.rightMargin: 20
                    color: "transparent"
                    z: 3

                    Image {
                        id: deviceExtendStatusImage
                        width: 16
                        height: 16
                        anchors.top: parent.top
                        source: "qrc:/images/extend.svg"
                        // 根据模型中的 extended 状态旋转图标
                        rotation: extended ? 0 : 90
                        Behavior on rotation {
                            PropertyAnimation { duration: 250 }
                        }
                    }

                    MouseArea {
                        id: userDeviceInformationExtendMouseArea
                        anchors.fill: parent
                        onClicked: {
                            // 更新 ListModel 中该项的 extended 状态
                            var currentData = deviceInformationModel.get(index);
                            deviceInformationModel.set(index, {
                                deviceName: currentData.deviceName,
                                deviceIP: currentData.deviceIP,
                                account: currentData.account,
                                password: currentData.password,
                                isConnected: currentData.isConnected,
                                extended: !currentData.extended
                            });
                        }
                    }
                }

                // 右侧设备信息图标
                Rectangle {
                    id: userDeviceInformationIcon
                    width: 16
                    height: 16
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: deviceExtendStatusRectangle.left
                    anchors.rightMargin: parent.width * 0.05
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    color: "transparent"

                    Image {
                        width: 16
                        height: 16
                        anchors.top: parent.top
                        source: "qrc:/images/info.svg"
                    }

                    MouseArea {
                        id: userDeviceItemInformation
                        anchors.fill: parent
                        z: 2
                        onClicked: {
                            console.log("OS:Windows 10" + '\n' + "CPU:Intel i5-8500")
                        }
                    }
                }

                // 基本信息区域内的右键菜单
                MouseArea {
                    id: deviceInformatioArea
                    anchors.fill: parent
                    z: 5
                    propagateComposedEvents: true
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onPressed: function(event) {
                        if (event.button === Qt.RightButton) {
                            showContextMenu(event.x, event.y);
                            event.accepted = true;
                        } else {
                            event.accepted = false;
                        }
                    }

                    function showContextMenu(x, y) {
                        contextMenu.x = x;
                        contextMenu.y = y;
                        contextMenu.popup();
                    }
                }

                Menu {
                    id: contextMenu
                    MenuItem {
                        text: "获取远端应用列表"
                        onTriggered: {
                            console.log("获取远端应用列表")
                            applistpage.show()
                        }
                    }
                    MenuItem {
                        text: "安装软件"
                        onTriggered: {
                            console.log("安装软件")
                        }
                    }
                    MenuItem {
                        text: "删除设备"
                        onTriggered: {
                            console.log("删除设备")
                            deviceInformationModel.remove(index)
                        }
                    }
                    MenuItem {
                        text: "配置设备"
                        onTriggered: {
                            var currentData = deviceInformationModel.get(index)
                            ipField.text = currentData.deviceIP
                            accountField.text = currentData.account
                            passwordField.text = currentData.password
                            inputWindow.currentIndex = index  // 保存当前索引
                            inputWindow.show()
                        }
                    }
                }
            } // end 基本信息区域

            // 展开详情区域，只有在 extended 为 true 时可见
            Rectangle {
                id: userDeviceDetailsInformationRectangle
                width: parent.width - 2
                // 使用剩余高度（当前项高度减去基本信息区域高度和间隔）作为展开内容高度
                height: deviceListItem.height - normalHeight - 5
                anchors.top: deviceInformationRectangle.bottom
                anchors.topMargin: 5
                anchors.horizontalCenter: parent.horizontalCenter
                visible: extended

                Flickable {
                    id: flickable
                    width: parent.width
                    height: parent.height - 40
                    contentWidth: parent.width
                    contentHeight: parent.height - 40
                    clip: false

                    Rectangle {
                        id: userDeviceAppListRectangle
                        width: flickable.width
                        height: 160

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

                                    Image {
                                        id: appIcon
                                        width: 24
                                        height: 24
                                        source: model.AppIconPath
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: parent.left
                                        anchors.leftMargin: 20
                                    }

                                    Text {
                                        id: appName
                                        text: model.AppName
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: appIcon.right
                                        anchors.leftMargin: 10
                                        anchors.rightMargin: 10
                                    }
                                }
                            }
                        }
                    }
                    flickableDirection: Flickable.VerticalFlick
                }
            } // end 展开详情区域
        } // end delegate
    } // end ListView

    // ListView 外部右键区域（当点击空白区域时显示菜单）
    MouseArea {
        id: userDeviceArea
        z: 3
        anchors.fill: parent
        propagateComposedEvents: true
        acceptedButtons: Qt.RightButton

        onPressed: function(event) {
            if (event.button === Qt.RightButton) {
                var item = userDeviceList.itemAt(event.x, event.y);
                if (!item) {
                    showContextMenu(event.x, event.y);
                    event.accepted = true;
                } else {
                    event.accepted = false;
                }
            }
        }

        function showContextMenu(x, y) {
            userDeviceMenu.x = x;
            userDeviceMenu.y = y;
            userDeviceMenu.popup();
        }
    }

    Menu {
        id: userDeviceMenu
        MenuItem {
            text: "添加新的远程设备"
            onTriggered: {
                console.log("添加新的远程设备")
                showInputDialog()
            }
        }
    }

    Window {
        id: inputWindow
        property int currentIndex: -1   // 保存当前操作的索引
        title: "添加新设备"
        width: 300
        height: 250
        modality: Qt.ApplicationModal
        visible: false
        flags: Qt.Window
        color: "white"

        Column {
            anchors.centerIn: parent
            anchors.topMargin: 20
            spacing: 10

            TextField {
                id: ipField
                placeholderText: "请输入IP地址"
                width: parent.width
            }

            TextField {
                id: accountField
                placeholderText: "请输入账户"
                width: parent.width
            }

            TextField {
                id: passwordField
                placeholderText: "请输入密码"
                echoMode: TextInput.Password
                width: parent.width
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10

                Button {
                    text: "确定"
                    onClicked: {
                        var ip = ipField.text
                        var account = accountField.text
                        var password = passwordField.text

                        if (ip && account && password) {
                            if (inputWindow.currentIndex >= 0) {
                                var currentData = deviceInformationModel.get(inputWindow.currentIndex);
                                deviceInformationModel.set(inputWindow.currentIndex, {
                                    deviceName: ip,
                                    deviceIP: ip,
                                    account: account,
                                    password: password,
                                    isConnected: currentData.isConnected,
                                    extended: currentData.extended
                                })
                            } else {
                                deviceInformationModel.append({
                                    deviceName: ip,
                                    deviceIP: ip,
                                    account: account,
                                    password: password,
                                    isConnected: true,
                                    extended: false
                                })
                            }
                            console.log("设备已添加/更新:", ip, account, password)
                            ipField.text = ""
                            accountField.text = ""
                            passwordField.text = ""
                            inputWindow.currentIndex = -1
                            inputWindow.close()
                        } else {
                            console.log("输入不完整，无法添加设备")
                        }
                    }
                }

                Button {
                    text: "取消"
                    onClicked: {
                        console.log("添加设备操作已取消")
                        inputWindow.close()
                    }
                }
            }
        }
    }

    function showInputDialog() {
        inputWindow.show()
    }

    RemoteAppList {
        id: applistpage
    }
}
