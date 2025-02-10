import QtQuick

Item {
    ListView {
        id: listView
        anchors.fill: parent
        model: softwareManager.softwareList
        delegate: Item {
            width: parent.width
            height: 50
            Column {
                spacing: 5
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 10

                Text {
                    text: model.name
                    font.bold: true
                    font.pixelSize: 14
                }
                Text {
                    text: "版本: " + (model.version ? model.version : "未知")
                    font.pixelSize: 12
                }
                Text {
                    text: "安装日期: " + (model.installDate ? model.installDate : "未知")
                    font.pixelSize: 12
                }
            }
        }
    }
}
