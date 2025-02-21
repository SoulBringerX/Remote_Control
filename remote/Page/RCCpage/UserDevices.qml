import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "../../Model" // 进入 Model 文件夹
Rectangle{
    property int isExtendCount: 0
    color: "transparent"
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
                                isExtendCount++
                                userDeviceInformationExtend.running = true
                                directionIconchanged1.running = true
                                iconchanged = true
                            }
                            else
                            {
                                isExtendCount--
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
                    color: "transparent"
                    Image{
                        width: 16
                        height: 16
                        anchors.top: parent.top
                        source: "qrc:/images/info.svg"
                    }
                    MouseArea{
                        id: userDeviceItemInformation
                        anchors.fill: parent
                        z: 2
                        onClicked: {
                            userDeviceInformation.text = "OS:Windosw 10" + '\n'+"CPU:Intel i5-8500"
                        }
                    }
                }
                Rectangle {
                    id: userDeviceDetailsInformationRectangle
                    width: parent.width - 2
                    height: parent.height
                    anchors.top: deviceExtendStatusRectangle.bottom
                    anchors.topMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: userDeviceInformationExtendMouseArea.iconchanged

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

                MouseArea {
                    id: deviceInformatioArea
                    anchors.fill: parent
                    z: 5
                    propagateComposedEvents: true
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onPressed: function(event) {
                        console.log("Mouse button pressed:", event.button);
                        if (event.button === Qt.RightButton) {
                            showContextMenu(event.x, event.y);
                            event.accepted = true; // 阻止事件传递到父级MouseArea
                        } else {
                            event.accepted = false; // 允许左键事件传递
                        }
                    }

                    function showContextMenu(x, y) {
                        contextMenu.x = x;
                        contextMenu.y = y;
                        contextMenu.popup();
                    }
                }

                // 定义上下文菜单
                Menu {
                    id: contextMenu

                    MenuItem {
                        text: "获取远端应用列表"
                        onTriggered: {
                            console.log("获取远端应用列表")
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
                        }
                    }
                    MenuItem {
                        text: "配置设备"
                        onTriggered: {
                            console.log("配置设备")
                        }
                    }
                }

            }
        PropertyAnimation
        {
            id: userDeviceInformationExtend
            target: deviceListItem
            properties: "height"
            from: 40
            to: userDeviceList.height / isExtendCount - (40 * (userDeviceList.count - isExtendCount))
            duration: 250
        }
        PropertyAnimation
        {
            id: userDeviceInformationShrink
            target: deviceListItem
            properties: "height"
            from: userDeviceList.height /  isExtendCount - (40 * (userDeviceList.count - isExtendCount))
            to: 40
            duration: 250
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
                    if (!item) { // 点击在空白区域
                        showContextMenu(event.x, event.y);
                        event.accepted = true;
                    } else {
                        event.accepted = false; // 由列表项处理
                    }
                }
            }

            function showContextMenu(x, y) {
                userDeviceMenu.x = x;
                userDeviceMenu.y = y;
                userDeviceMenu.popup();
            }
        }
        // 定义上下文菜单
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
    }
    Window {
        id: inputWindow
        title: "添加新设备"
        width: 300
        height: 250
        modality: Qt.ApplicationModal
        visibility: "Windowed"
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
                            deviceModel.append({ ip: ip, account: account, password: password });
                            console.log("新设备已添加:", ip, account, password);
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

    // 显示输入框
    function showInputDialog() {
        inputWindow.show();
    }
}
