import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    width: 640
    height: 480
    visible: false
    title: "远程应用列表"

    // 应用信息模型
    ListModel {
        id: appListModel
    }

    // 应用列表视图
    ListView {
        id: appListView
        anchors.fill: parent
        anchors.bottomMargin: 60
        model: appListModel

        delegate: Item {
            id: appListItem
            width: parent.width
            height: 40

            Rectangle {
                id: appItem
                anchors.fill: parent
                border.color: "black"
                border.width: 0.5
                color: "transparent"

                Image {
                    id: appIcon
                    width: 16
                    height: 16
                    source: AppIconPath
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                }

                Text {
                    id: appName
                    text: AppName
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: appIcon.right
                    anchors.leftMargin: 10
                }

                CheckBox {
                    id: appCheckBox
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    checked: isSelected
                    onCheckedChanged: {
                        appListModel.setProperty(index, "isSelected", checked)
                    }
                }
            }
        }
    }

    // 底部按钮
    Row {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottomMargin: 10

        Button {
            text: "添加勾选"
            width: parent.width / 2 - 10
            onClicked: {
                // 添加勾选逻辑
                console.log("添加勾选")
            }
        }

        Button {
            text: "一键勾选"
            width: parent.width / 2 - 10
            onClicked: {
                // 一键勾选逻辑
                for (var i = 0; i < appListModel.count; i++) {
                    appListModel.setProperty(i, "isSelected", true)
                }
            }
        }
    }

    // 在窗口打开时发送消息并接收应用列表
    Component.onCompleted: {
        tcpConnection.sendPacket(requestPacket)
        var appList = tcpConnection.receiveAppList()
        for (var i = 0; i < appList.length; i++) {
            appListModel.append({
                AppName: appList[i].name,
                AppIconPath: "data:image/png;base64," + appList[i].iconData,
                isSelected: false
            })
        }
    }

    // 定义一个 RD_Packet 对象用于发送请求
    property var requestPacket: {
        RD_Type: OperationCommandType.TransmitAppAlias
    }
}
