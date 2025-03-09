import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: container
    width: 800
    height: 600
    color: "#f0f0f0"

    // 辅助函数：重复字符串（替代 ES6 的 repeat() 方法）
    function repeat(str, count) {
        var res = "";
        for (var i = 0; i < count; i++) {
            res += str;
        }
        return res;
    }

    // 设备数据模型
    ListModel {
        id: deviceInformationModel
        ListElement {
            deviceName: "Computer 1"
            deviceIP: "192.168.1.1"
            account: "user1"
            password: "pass1"
            isConnected: true
            isExpanded: false
        }
        ListElement {
            deviceName: "Computer 2"
            deviceIP: "192.168.1.2"
            account: "user2"
            password: "pass2"
            isConnected: false
            isExpanded: false
        }
    }

    // 主列表视图
    ListView {
        id: deviceListView
        anchors.fill: parent
        spacing: 2
        model: deviceInformationModel
        interactive: false

        delegate: Rectangle {
            id: listItem
            // 定义本地属性：初始高度与展开时高度（计算函数可根据需要调整）
            property real baseHeight: 40
            property real expandedHeight: calculateExpandedHeight()
            // 使用模型中 isExpanded 属性；注意此处直接使用 isExpanded（模型角色自动导入）
            // 如需在 delegate 内部修改，则会使用 deviceInformationModel.setProperty(...)
            width: deviceListView.width
            height: isExpanded ? expandedHeight : baseHeight
            color: isExpanded ? "#e3f2fd" : "white"
            border.color: "#b0bec5"

            Behavior on height { NumberAnimation { duration: 200 } }
            Behavior on color { ColorAnimation { duration: 200 } }

            // 头部区域
            RowLayout {
                width: listItem.width - 20
                height: 40
                anchors.centerIn: listItem
                spacing: 15

                Image {
                    source: "qrc:/icons/computer.svg"
                    Layout.preferredWidth: 24
                    Layout.preferredHeight: 24
                }

                Text {
                    text: deviceName      // 来自模型的 deviceName 属性
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }

                Button {
                    id: expandButton
                    icon.source: "qrc:/icons/expand.svg"
                    icon.width: 16
                    icon.height: 16
                    rotation: isExpanded ? 180 : 0
                    flat: true
                    onClicked: {
                        // 切换展开状态，并更新模型数据
                        isExpanded = !isExpanded
                        deviceInformationModel.setProperty(index, "isExpanded", isExpanded)
                    }
                    Behavior on rotation { NumberAnimation { duration: 200 } }
                }
            }

            // 展开内容区域
            Column {
                visible: isExpanded
                width: listItem.width - 40
                anchors.top: listItem.top
                anchors.topMargin: 50
                anchors.horizontalCenter: listItem.horizontalCenter
                spacing: 10

                // 连接状态指示器
                Row {
                    spacing: 10
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 6
                        color: isConnected ? "#4caf50" : "#f44336"
                    }
                    Text {
                        text: isConnected ? "已连接" : "未连接"
                        color: "#607d8b"
                    }
                }

                // 详细信息表格
                GridLayout {
                    columns: 2
                    columnSpacing: 20
                    rowSpacing: 8

                    Text { text: "IP地址:"; color: "#78909c" }
                    Text { text: deviceIP }
                    Text { text: "账户:"; color: "#78909c" }
                    Text { text: account }
                    Text { text: "密码:"; color: "#78909c" }
                    Text { text: repeat("•", password.length) }
                }

                // 操作按钮组
                Row {
                    spacing: 15
                    Button {
                        text: "连接设备"
                        onClicked: connectDevice(index)
                    }
                    Button {
                        text: "删除设备"
                        onClicked: deviceInformationModel.remove(index)
                    }
                }
            }

            // 右键菜单（在列表项内）
            MouseArea {
                anchors.fill: listItem
                acceptedButtons: Qt.RightButton
                onClicked: itemMenu.popup()

                Menu {
                    id: itemMenu
                    MenuItem {
                        text: "编辑配置"
                        onTriggered: showEditor(index)
                    }
                    MenuItem {
                        text: "克隆设备"
                        onTriggered: cloneDevice(index)
                    }
                }
            }
        }
    }

    // 计算展开项的高度：根据 ListView 的高度和当前折叠项数量进行动态计算
    function calculateExpandedHeight() {
        var totalHeight = deviceListView.height;
        var collapsed = 0;
        for (var i = 0; i < deviceInformationModel.count; i++) {
            if (!deviceInformationModel.get(i).isExpanded)
                collapsed++;
        }
        var expandedCount = deviceInformationModel.count - collapsed;
        if(expandedCount === 0)
            return 0;
        return (totalHeight - (collapsed * 40)) / expandedCount;
    }

    // 设备连接逻辑（示例）
    function connectDevice(index) {
        var device = deviceInformationModel.get(index);
        console.log("正在连接:", device.deviceIP);
        // 模拟连接后设置为已连接状态
        deviceInformationModel.setProperty(index, "isConnected", true);
    }

    // 设备克隆功能
    function cloneDevice(index) {
        var original = deviceInformationModel.get(index);
        deviceInformationModel.append({
            deviceName: original.deviceName + " - 副本",
            deviceIP: original.deviceIP,
            account: original.account,
            password: original.password,
            isConnected: false,
            isExpanded: false
        });
    }

    // 编辑设备配置（示例）
    function showEditor(index) {
        console.log("编辑设备配置:", deviceInformationModel.get(index));
        // 可在此处添加打开编辑窗口的逻辑
    }

    // 右键添加设备菜单
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: contextMenu.popup()

        Menu {
            id: contextMenu
            MenuItem {
                text: "添加设备"
                onTriggered: deviceConfigWindow.show()
            }
        }
    }

    // 设备配置窗口
    Window {
        id: deviceConfigWindow
        title: "设备配置"
        width: 300
        height: 250
        modality: Qt.WindowModal

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8

            TextField { id: nameField; placeholderText: "设备名称" }
            TextField { id: ipField; placeholderText: "IP地址" }
            TextField { id: userField; placeholderText: "用户名" }
            TextField { id: passField; placeholderText: "密码"; echoMode: TextInput.Password }

            RowLayout {
                Button {
                    text: "保存"
                    onClicked: {
                        deviceInformationModel.append({
                            deviceName: nameField.text,
                            deviceIP: ipField.text,
                            account: userField.text,
                            password: passField.text,
                            isConnected: false,
                            isExpanded: false
                        });
                        deviceConfigWindow.close();
                    }
                }
                Button {
                    text: "取消"
                    onClicked: deviceConfigWindow.close()
                }
            }
        }
    }
}
