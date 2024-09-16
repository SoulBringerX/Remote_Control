import QtQuick 2.15
import QtQuick.Controls

ListModel {
    id: deviceInformationModel
    ListElement { deviceName: "Computer 1"; isConnected: true }
    ListElement { deviceName: "Computer 2"; isConnected: false }
    ListElement { deviceName: "Computer 3"; isConnected: true }
}
