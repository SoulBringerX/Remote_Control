import QtQuick

Window {
    id: systemErrorWindow
    maximumWidth: 450
    maximumHeight: 120
    minimumWidth: 450
    minimumHeight: 120
    title:"SystemError"

    Rectangle
    {
        id: registerDialogIcon
        width: 32
        height: 32
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 15
        anchors.topMargin: 55
        Image
        {
            source: "qrc:/images/系统错误.svg"
            anchors.fill: parent
        }
    }
    Text{
        anchors.left: registerDialogIcon.right
        anchors.top: registerDialogIcon.top
        anchors.leftMargin: 10
        font.pointSize: 16
        text: "ERROR："
    }
}
