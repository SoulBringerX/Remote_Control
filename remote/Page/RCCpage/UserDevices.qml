import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import "../../Model" // 进入 Model 文件夹
import "."

Rectangle {
    id: root
    color: "transparent"

    // 设备数据模型，记录设备基本信息、扩展状态以及应用列表（存储在 userApps 字段中）
    ListModel {
        id: deviceInformationModel
    }
    DeviceAppModel {
        id: deviceAppListModel
    }
    property int currentIndex: -1
    property string currentIp: ""
    // 选中状态
    property bool isSelected: false
    // 悬停状态
    property bool isHovered: false

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

            // 当该项展开时，高度 = 基本高度 + (剩余高度平均分给所有展开项)
            height: extended
                     ? normalHeight + ((userDeviceList.height - (deviceInformationModel.count * normalHeight)) / (calcExpandedCount() > 0 ? calcExpandedCount() : 1))
                     : normalHeight

            Behavior on height { PropertyAnimation { duration: 250 } }

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

                // 右上角展开/收缩图标
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
                            // 切换 expanded 状态，同时保留原有的 userApps 字段
                            var currentData = deviceInformationModel.get(index);
                            currentIndex = index;
                            deviceInformationModel.set(index, {
                                deviceName: currentData.deviceName,
                                deviceIP: currentData.deviceIP,
                                account: currentData.account,
                                password: currentData.password,
                                isConnected: currentData.isConnected,
                                extended: !currentData.extended,
                                userApps: currentData.userApps ? currentData.userApps : []
                            });

                            // 如果设备展开，则加载应用列表
                            if (deviceInformationModel.get(index).extended) {
                                var ip = deviceInformationModel.get(index).deviceIP;
                                var appList = user_device.loadAppfromini(ip);
                                deviceAppListModel.clear();
                                for (var i = 0; i < appList.length; i++) {
                                    deviceAppListModel.append(appList[i]);
                                }
                            }
                        }
                    }
                }

                // 右侧设备信息图标
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
                            console.log("OS:Windows 10\nCPU:Intel i5-8500")
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
                            console.log("获取远端应用列表");
                            currentIndex = index;
                            currentIp = deviceInformationModel.get(currentIndex).deviceIP;
                            // 将设备 IP 传给 RemoteAppList
                            applistpage.ip = currentIp;
                            applistpage.visible = true;
                        }
                    }
                    MenuItem { text: "安装软件"; onTriggered: { console.log("安装软件") } }
                    MenuItem {
                        text: "删除设备"
                        onTriggered: {
                            console.log("删除设备");
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
            } // end deviceInformationRectangle

            // 展开详情区域：显示应用列表，绑定 userApps 属性
            Rectangle {
                id: userDeviceDetailsInformationRectangle
                width: parent.width - 2
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
                    interactive: true  // 确保交互启用

                    Rectangle {
                        id: userDeviceAppListRectangle
                        width: flickable.width
                        height: 160

                        ListView {
                            id: deviceAppListView
                            anchors.fill: parent
                            model: deviceAppListModel
                            delegate: Item {
                                id: appItem
                                width: userDeviceAppListRectangle.width
                                height: 32
                                property bool isHovered: false
                                property bool isSelected: false

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
                                        propagateComposedEvents: true

                                        // 悬停检测
                                        onEntered: appItem.isHovered = true
                                        onExited: appItem.isHovered = false

                                        // 单击选中
                                        onClicked: {
                                            deviceAppListView.currentIndex = index
                                            appItem.isSelected = true
                                        }

                                        // 双击打开
                                        onDoubleClicked: {
                                            console.log("打开应用:", model.AppName)
                                            // 调用远程打开接口
                                        }

                                        // 右键菜单
                                        onPressed: function(mouse) {  // 明确声明 mouse 参数
                                            if (mouse.button === Qt.RightButton) {
                                                // 设置菜单显示位置为鼠标点击位置
                                                appContextMenu.x = mouse.x;
                                                appContextMenu.y = mouse.y;
                                                appContextMenu.popup();
                                            }
                                        }
                                    }

                                    MouseArea {
                                        id: appitemMouseArea
                                        anchors.fill: parent
                                        propagateComposedEvents: true

                                        // 悬停检测
                                        onEntered: appItem.isHovered = true
                                        onExited: appItem.isHovered = false

                                        // 单击选中
                                        onClicked: {
                                            deviceAppListView.currentIndex = index
                                            appItem.isSelected = true
                                        }

                                        // 双击打开
                                        onDoubleClicked: {
                                            console.log("打开应用:", model.AppName)
                                            // 调用远程打开接口
                                        }

                                        // 右键菜单
                                        onPressed: function(mouse) {  // 明确声明 mouse 参数
                                            if (mouse.button === Qt.RightButton) {
                                                // 设置菜单显示位置为鼠标点击位置
                                                appContextMenu.x = mouse.x;
                                                appContextMenu.y = mouse.y;
                                                appContextMenu.popup();
                                            }
                                        }
                                    }

                                    // 应用右键菜单
                                    Menu {
                                        id: appContextMenu
                                        property var appData: model  // 绑定当前应用数据
                                        property string deviceIp: currentIp  // 绑定当前设备 IP

                                        MenuItem {
                                            text: "打开应用"
                                            onTriggered: {
                                                remoteControlThread.startApp(
                                                    appContextMenu.deviceIp,
                                                    appContextMenu.appData.RdpAppName,
                                                    account,
                                                    password
                                                )
                                            }
                                        }

                                        MenuItem {
                                            text: "卸载应用"
                                            onTriggered: {
                                                console.log("卸载应用:", appContextMenu.appData.AppName)
                                                user_device.uninstallApp(
                                                    appContextMenu.deviceIp,
                                                    appContextMenu.appData.RdpAppName
                                                )
                                                deviceAppListModel.remove(index)
                                            }
                                        }

                                        MenuItem {
                                            text: "删除本地记录"
                                            onTriggered: {
                                                console.log("删除应用记录:", appContextMenu.appData.AppName)
                                                deviceAppListModel.remove(index)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    flickableDirection: Flickable.VerticalFlick
                }
            } // end userDeviceDetailsInformationRectangle
        } // end delegate
    } // end ListView

    // 空白区域右键显示菜单
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
                console.log("添加新的远程设备");
                showInputDialog();
            }
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
                        var ip = ipField.text;
                        var account = accountField.text;
                        var password = passwordField.text;
                        if (ip && account && password) {
                            if (inputWindow.currentIndex >= 0) {
                                var currentData = deviceInformationModel.get(inputWindow.currentIndex);
                                deviceInformationModel.set(inputWindow.currentIndex, {
                                    deviceName: ip,
                                    deviceIP: ip,
                                    account: account,
                                    password: password,
                                    isConnected: currentData.isConnected,
                                    extended: currentData.extended,
                                    userApps: currentData.userApps ? currentData.userApps : []
                                });
                                user_device.sendUserDevice(ip, account, password);
                            } else {
                                deviceInformationModel.append({
                                    deviceName: ip,
                                    deviceIP: ip,
                                    account: account,
                                    password: password,
                                    isConnected: true,
                                    extended: false,
                                    userApps: []
                                });
                                user_device.sendUserDevice(ip, account, password);
                            }
                            console.log("设备已添加/更新:", ip, account, password);
                            ipField.text = "";
                            accountField.text = "";
                            passwordField.text = "";
                            inputWindow.currentIndex = -1;
                            inputWindow.close();
                        } else {
                            console.log("输入不完整，无法添加设备");
                        }
                    }
                }
                Button {
                    text: "取消"
                    onClicked: {
                        console.log("添加设备操作已取消");
                        inputWindow.close();
                    }
                }
            }
        }
    }

    function showInputDialog() {
        inputWindow.show();
    }

    // 加载设备数据时，检查并确保 userApps 为数组（若存在冲突则重置）
    Component.onCompleted: {
        var devices = user_device.getUserDevices();
        deviceInformationModel.clear();
        devices.forEach(function(device) {
            // 创建 ListModel 存储应用
            var appsModel = Qt.createQmlObject('import QtQuick 2.15; ListModel {}', root);
            if (device.userApps) {
                device.userApps.forEach(function(app) {
                    appsModel.append(app);
                });
            }
            // 将 ListModel 存入设备信息
            deviceInformationModel.append({
                deviceName: device.deviceName,
                deviceIP: device.deviceIP,
                account: device.account,
                password: device.password,
                isConnected: device.isConnected,
                extended: false,
                userApps: appsModel  // ✅ 存储 ListModel 对象
            });
        });
    }

    RemoteAppList {
        id: applistpage
        ip: currentIp
    }

    // 删除设备确认弹窗
    Dialog {
        id: confirmDeleteDialog
        title: "确认删除"
        visible: false
        property string currentIp: ""
        contentItem: Text {
            text: "是否确认删除该设备？远程服务器上的相关数据也将被删除。"
        }
        footer: DialogButtonBox {
            Button {
                text: "确认"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                onClicked: {
                    console.log("用户确认删除");
                    var currentDevice = deviceInformationModel.get(currentIndex);
                    currentIp = currentDevice.deviceIP;
                    console.log("当前设备 IP:", currentIp);
                    user_device.deleteUserDevice(currentIp);
                    deviceInformationModel.remove(currentIndex);
                    currentIndex = -1;
                    confirmDeleteDialog.accept();
                }
            }
            Button {
                text: "取消"
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                onClicked: {
                    console.log("用户取消删除");
                    confirmDeleteDialog.reject();
                }
            }
        }
    }
}
