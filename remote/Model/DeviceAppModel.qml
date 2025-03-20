import QtQuick 2.15

ListModel {
    id: deviceAppListModel
    ListElement {
        deviceIP: "192.168.1.1"
        deviceName: "Computer 1"
        AppName: "App 1";
        RdpAppName: "App 1"
        AppIconPath: "qrc:/images/QQ.svg"
        isConnected: true
    }
    ListElement {
        deviceIP: "192.168.1.1"
        deviceName: "Computer 1"
        AppName: "App 2"
        RdpAppName: "App2"
        AppIconPath: "qrc:/images/wx.svg"
        isConnected: true
    }
}
