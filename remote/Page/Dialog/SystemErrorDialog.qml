import QtQuick

Window {
    id: systemErrorDialog
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
        anchors.leftMargin: 15
        anchors.verticalCenter: parent.verticalCenter
        Image
        {
            source: "qrc:/images/SystemError.svg"
            anchors.fill: parent
        }
    }
    Text{
        anchors.left: registerDialogIcon.right
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: 16
        text: "ERROR："
    }
}
