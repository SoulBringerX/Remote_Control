import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Popup {
    id: remoteAppPopup
    width: 640
    height: 480
    modal: true
    focus: true
    // 按 Esc 键或点击弹窗外部时自动关闭
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property string ip: ""

    // 应用信息模型
    ListModel {
        id: appListModel
    }

    // 主体布局
    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // 顶部区域：包括关闭按钮
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            Button {
                text: "关闭"
                onClicked: remoteAppPopup.close()
            }
        }

        // 应用列表视图
        ListView {
            id: appListView
            Layout.fillWidth: true
            Layout.fillHeight: true
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

        // 底部按钮区域
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10

            Button {
                text: "添加勾选"
                width: parent.width / 2 - 10
                onClicked: {
                    console.log("添加勾选")
                }
            }

            Button {
                text: "一键勾选"
                width: parent.width / 2 - 10
                onClicked: {
                    for (var i = 0; i < appListModel.count; i++) {
                        appListModel.setProperty(i, "isSelected", true)
                    }
                }
            }
        }
    }

    // 在弹窗打开时发送消息并接收应用列表
    Component.onCompleted: {
        console.log("当前IP： " + ip)
        // 请确保 tcpConnection 已经在上下文中定义
        if (typeof tcpConnection !== "undefined") {
            // 先连接远程设备
            if (tcpConnection.connect("192.168.31.84")) {
                // 连接成功后发送请求
                tcpConnection.sendPacket(requestPacket)
                var appList = tcpConnection.receiveAppList()
                for (var i = 0; i < appList.length; i++) {
                    appListModel.append({
                        AppName: appList[i].name,
                        AppIconPath: "data:image/png;base64," + appList[i].iconData,
                        isSelected: false
                    })
                }
            } else {
                console.log("连接远程设备失败")
            }
        } else {
            console.log("tcpConnection 未定义")
        }
    }

    // 定义一个请求包对象用于发送请求
    property var requestPacket: {
        RD_Type: OperationCommand.TransmitAppAlias
    }
}
