import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import "../../Model"
import "."

Rectangle {
    id: root
    color: "transparent"

    ListModel {
        id: deviceInformationModel
    }
    property int currentIndex: -1
    property string currentIp: ""

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
            property int normalHeight: 40

            height: extended
                     ? normalHeight + ((userDeviceList.height - (deviceInformationModel.count * normalHeight)) / (calcExpandedCount() > 0 ? calcExpandedCount() : 1))
                     : normalHeight

            Behavior on height { PropertyAnimation { duration: 250 } }

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
                    anchors.top: parent.top; anchors.topMargin: 10
                    anchors.left: parent.left; anchors.leftMargin: 20
                }

                Text {
                    id: deviceNameText
                    anchors.left: deviceIconImage.right; anchors.leftMargin: 10
                    text: qsTr(deviceName)
                    anchors.top: parent.top; anchors.topMargin: 10
                    MouseArea {
                        id: deviceNameMouseArea
                        anchors.fill: parent
                        onDoubleClicked: {
                            console.log("Connecting to device:", deviceIP)
                            remoteControlThread.startConnection(deviceIP, account, password)
                            remoteView.visible = true
                        }
                    }
                }

                RemoteDraw { id: remoteView }

                Rectangle {
                    id: deviceExtendStatusRectangle
                    width: 16; height: 16
                    anchors.right: parent.right; anchors.top: parent.top
                    anchors.topMargin: 10; anchors.rightMargin: 20
                    color: "transparent"; z: 3

                    Image {
                        id: deviceExtendStatusImage
                        width: 16; height: 16
                        anchors.top: parent.top
                        source: "qrc:/images/extend.svg"
                        rotation: extended ? 0 : 90
                        Behavior on rotation { PropertyAnimation { duration: 250 } }
                    }

                    MouseArea {
                        id: userDeviceInformationExtendMouseArea
                        anchors.fill: parent
                        onClicked: {
                            var currentData = deviceInformationModel.get(index);
                            currentIndex = index;
                            deviceInformationModel.set(index, {
                                deviceName: currentData.deviceName,
                                deviceIP: currentData.deviceIP,
                                account: currentData.account,
                                password: currentData.password,
                                isConnected: currentData.isConnected,
                                extended: !currentData.extended,
                                userApps: currentData.userApps
                            });

                            if (deviceInformationModel.get(index).extended) {
                                var ip = currentData.deviceIP;
                                if (currentData.userApps.count === 0) {
                                    var appList = user_device.loadAppfromini(ip);
                                    currentData.userApps.clear();
                                    for (var i = 0; i < appList.length; i++) {
                                        currentData.userApps.append(appList[i]);
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    id: userDeviceInformationIcon
                    width: 16; height: 16
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: deviceExtendStatusRectangle.left
                    anchors.rightMargin: parent.width * 0.05
                    anchors.top: parent.top; anchors.topMargin: 10
                    color: "transparent"

                    Image {
                        width: 16; height: 16
                        anchors.top: parent.top
                        source: "qrc:/images/info.svg"
                    }

                    MouseArea {
                        id: userDeviceItemInformation
                        anchors.fill: parent
                        z: 2
                        onClicked: {
                            tcp.connectToServer(deviceIP);
                            var deviceInfo = tcp.receiveDeviceInfo();
                        }
                    }
                }

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
                            currentIndex = index;
                            currentIp = deviceInformationModel.get(currentIndex).deviceIP;
                            applistpage.ip = currentIp;
                            applistpage.visible = true;
                        }
                    }
                    MenuItem { text: "安装软件"; onTriggered: { console.log("安装软件") } }
                    MenuItem {
                        text: "删除设备"
                        onTriggered: {
                            currentIndex = index;
                            confirmDeleteDialog.open();
                        }
                    }
                    MenuItem {
                        text: "配置设备"
                        onTriggered: {
                            var currentData = deviceInformationModel.get(index);
                            ipField.text = currentData.deviceIP;
                            accountField.text = currentData.account;
                            passwordField.text = currentData.password;
                            inputWindow.currentIndex = index;
                            inputWindow.show();
                        }
                    }
                }
            }

            Rectangle {
                id: userDeviceDetailsInformationRectangle
                width: parent.width - 2
                height: extended ? (deviceListItem.height - normalHeight) : 0
                anchors.top: deviceInformationRectangle.bottom
                anchors.topMargin: 5
                anchors.horizontalCenter: parent.horizontalCenter
                visible: extended

                Flickable {
                    id: flickable
                    width: parent.width
                    height: parent.height
                    contentWidth: parent.width
                    contentHeight: userDeviceAppListRectangle.height
                    clip: true
                    interactive: true

                    Rectangle {
                        id: userDeviceAppListRectangle
                        width: flickable.width
                        height: Math.max(200, deviceListItem.height - normalHeight - 10)
                        color: "transparent"

                        ListView {
                            id: deviceAppListView
                            anchors.fill: parent
                            model: userApps  // 使用设备自己的应用列表模型
                            delegate: Item {
                                id: appItem
                                width: userDeviceAppListRectangle.width
                                height: 40

                                property bool isSelected: false
                                property bool isHovered: false

                                Rectangle {
                                    anchors.fill: parent
                                    border.color: appItem.isHovered ? "blue" : "transparent"
                                    color: appItem.isSelected ? "#e0e0e0" : "transparent"

                                    Image {
                                        id: appIcon
                                        width: 24; height: 24
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
                                    }

                                    MouseArea {
                                        id: appMouseArea
                                        anchors.fill: parent
                                        hoverEnabled: true  // 启用鼠标悬浮检测
                                        acceptedButtons: Qt.LeftButton | Qt.RightButton  // 接受左键和右键
                                        propagateComposedEvents: true

                                        onEntered: appItem.isHovered = true
                                        onExited: appItem.isHovered = false

                                        onClicked: function(mouse) {
                                            if (mouse.button === Qt.LeftButton) {
                                                deviceAppListView.currentIndex = index
                                                appItem.isSelected = !appItem.isSelected
                                            }
                                        }

                                        onDoubleClicked: function(mouse) {
                                            if (mouse.button === Qt.LeftButton) {
                                                console.log("打开应用:", model.AppName)
                                            }
                                        }

                                        onPressed: function(mouse) {
                                            if (mouse.button === Qt.RightButton) {
                                                appItem.isSelected = true  // 右键单击时显示灰色背景
                                                appContextMenu.appData = model
                                                appContextMenu.deviceIp = deviceInformationModel.get(currentIndex).deviceIP
                                                appContextMenu.x = mouse.x
                                                appContextMenu.y = mouse.y
                                                appContextMenu.popup()
                                                mouse.accepted = true
                                            }
                                        }
                                    }

                                    Menu {
                                        id: appContextMenu
                                        property var appData
                                        property string deviceIp

                                        // 菜单关闭时恢复未选中状态
                                        onClosed: {
                                            appItem.isSelected = false
                                        }

                                        MenuItem {
                                            text: "打开应用"
                                            onTriggered: {
                                                // 校验是否为鼠标选择的应用名称
                                                console.log("正在打开软件："+appName.text);
                                                // 这里先去发送应用的名称然后接受应用执行路径在去启动freerdp
                                            }
                                        }

                                        MenuItem {
                                            text: "卸载应用"
                                            onTriggered: {
                                                var currentData = deviceInformationModel.get(index);
                                                console.log("正在卸载软件："+appName.text);
                                                console.log("远程机器IP："+currentData.deviceIP);
                                                // user_device.uninstallApp(currentData.deviceIP, appData.RdpAppName)
                                                // 启动xfreerdp去远程卸载软件
                                                // userApps.remove(index)
                                            }
                                        }

                                        MenuItem {
                                            text: "删除本地记录"
                                            onTriggered: {
                                                userApps.remove(index)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

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
            onTriggered: showInputDialog()
        }
    }

    Window {
        id: inputWindow
        property int currentIndex: -1
        title: "添加新设备"
        width: 300; height: 250
        modality: Qt.ApplicationModal
        visible: false
        flags: Qt.Window
        color: "white"

        Column {
            anchors.centerIn: parent
            anchors.topMargin: 20
            spacing: 10

            TextField { id: ipField; placeholderText: "IP地址" }
            TextField { id: accountField; placeholderText: "账户" }
            TextField { id: passwordField; placeholderText: "密码"; echoMode: TextInput.Password }

            Row {
                spacing: 10
                Button {
                    text: "确定"
                    onClicked: {
                        if (ipField.text && accountField.text && passwordField.text) {
                            var newAppModel = Qt.createQmlObject('import QtQuick 2.15; ListModel {}', root);
                            if (inputWindow.currentIndex >= 0) {
                                deviceInformationModel.set(inputWindow.currentIndex, {
                                    deviceName: ipField.text,
                                    deviceIP: ipField.text,
                                    account: accountField.text,
                                    password: passwordField.text,
                                    isConnected: true,
                                    extended: false,
                                    userApps: newAppModel
                                });
                            } else {
                                deviceInformationModel.append({
                                    deviceName: ipField.text,
                                    deviceIP: ip.text,
                                    account: accountField.text,
                                    password: passwordField.text,
                                    isConnected: true,
                                    extended: false,
                                    userApps: newAppModel
                                });
                            }
                            user_device.sendUserDevice(ipField.text, accountField.text, passwordField.text);
                            inputWindow.close();
                        }
                    }
                }
                Button { text: "取消"; onClicked: inputWindow.close() }
            }
        }
    }

    Component.onCompleted: {
        var devices = user_device.getUserDevices();
        devices.forEach(function(device) {
            var appsModel = Qt.createQmlObject('import QtQuick 2.15; ListModel {}', root);
            if (device.userApps) {
                device.userApps.forEach(function(app) {
                    appsModel.append(app);
                });
            }
            deviceInformationModel.append({
                deviceName: device.deviceName,
                deviceIP: device.deviceIP,
                account: device.account,
                password: device.password,
                isConnected: device.isConnected,
                extended: false,
                userApps: appsModel
            });
        });
    }

    RemoteAppList { id: applistpage; ip: currentIp }

    Dialog {
        id: confirmDeleteDialog
        title: "确认删除"
        contentItem: Text { text: "是否确认删除该设备？" }
        footer: DialogButtonBox {
            Button {
                text: "确认"
                onClicked: {
                    var ip = deviceInformationModel.get(currentIndex).deviceIP;
                    user_device.deleteUserDevice(ip);
                    deviceInformationModel.remove(currentIndex);
                    confirmDeleteDialog.close();
                }
            }
            Button { text: "取消"; onClicked: confirmDeleteDialog.close() }
        }
    }
}
