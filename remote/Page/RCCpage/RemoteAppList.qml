import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    id: remoteAppListWindow
    width: 640
    height: 480
    visible: false
    title: "远程应用列表"

    // 当前设备 IP 与选中应用列表信号
    property string ip: ""

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
            width: parent.width
            height: 40

            Rectangle {
                anchors.fill: parent
                border.color: "black"
                border.width: 0.5
                color: "transparent"

                Image {
                    id: appIcon
                    width: 16
                    height: 16
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

                CheckBox {
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    checked: isSelected
                    onCheckedChanged: appListModel.setProperty(index, "isSelected", checked)
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
                var selectedApps = [];
                for (var i = 0; i < appListModel.count; i++) {
                    var app = appListModel.get(i);
                    if (app.isSelected) {
                        selectedApps.push({
                            AppName: app.AppName,
                            RdpAppName: app.AppName,
                            AppIconPath: app.AppIconPath
                        });
                    }
                }
                // 调用 C++ 方法保存应用列表
                if (selectedApps.length > 0) {
                    user_device.saveAppsToDevice(ip, selectedApps);
                } else {
                    console.log("没有勾选任何应用");
                }
            }
        }
        Button {
            text: "一键勾选"
            width: parent.width / 2 - 10
            onClicked: {
                var allApps = [];
                for (var i = 0; i < appListModel.count; i++) {
                    var app = appListModel.get(i);
                    appListModel.setProperty(i, "isSelected", true);
                    allApps.push({
                        AppName: app.AppName,
                        RdpAppName: app.AppName,
                        AppIconPath: app.AppIconPath
                    });
                }
                // 调用 C++ 方法保存所有应用
                if (allApps.length > 0) {
                    user_device.saveAppsToDevice(ip, allApps);
                } else {
                    console.log("没有应用可保存");
                }
            }
        }
    }

    // 当窗口打开时初始化数据
    onVisibleChanged: {
        if (visible) {
            appListModel.clear();
            if (tcp !== null) {
                tcp.connectToServer(ip);
                var appList = tcp.receiveAppList();
                for (var i = 0; i < appList.length; i++) {
                    appListModel.append({
                        AppName: appList[i].name,
                        AppIconPath: "data:image/png;base64," + appList[i].iconData,
                        isSelected: false
                    });
                }
            }
        }
    }
}
