import QtQuick

Window
{
    property bool registerstatus: true

    id: registerDialog
    title:"SystemCall"
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
            source: registerstatus ? "qrc:/images/OK.svg" : "qrc:/images/NO.svg"
            anchors.fill: parent
        }
    }
    Text{
        anchors.left: registerDialogIcon.right
        anchors.top: registerDialogIcon.top
        anchors.leftMargin: 10
        font.pointSize: 16
        text: registerstatus ? "账户注册完毕，请返回登录界面继续操作" : "账户注册失败，请确认是否存在网络或者输入问题"
    }
}
