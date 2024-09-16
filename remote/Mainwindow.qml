import QtQuick
import QtQuick.Controls

Window {
    id: root
    width: 1280
    height: 720
    visible: true
    title: qsTr("欢迎使用RemoteControl")

    Rectangle
    {
        id: userSideBar
        width: 150
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        color: "lightblue"
    }

    // 监听窗口关闭事件
    onClosing: {
        // 调用 Qt.quit() 来终止程序
        Qt.quit();
    }
}
