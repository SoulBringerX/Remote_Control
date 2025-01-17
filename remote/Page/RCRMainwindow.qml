import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Controls.Basic

Window {
    id: root
    maximumWidth: Screen.desktopAvailableWidth * 0.625
    maximumHeight: Screen.desktopAvailableHeight * 0.675
    minimumWidth: Screen.desktopAvailableWidth * 0.625
    minimumHeight: Screen.desktopAvailableHeight * 0.675
    visible: true
    title: qsTr("欢迎使用RemoteControl")
    flags: Qt.FramelessWindowHint | Qt.Window | Qt.WindowCloseButtonHint

    property bool registerstatus: true
    property int dragX: 0
    property int dragY: 0
    property bool dragging: false
    property bool isplaying: false

    signal minimizeToTray()

    // 主界面布局
    Row {
        id: mainLayout
        anchors.fill: parent

        // 左侧用户栏
        Rectangle {
            id: userPanel
            color: "#f5f5f5"
            width: parent.width * 0.25 // 25% 的宽度用于用户栏
            height: parent.height
            anchors.left: parent.left

            Column {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 10
                z:10

                // 用户信息和操作按钮
                Text {
                    text: qsTr("用户信息")
                    font.pixelSize: 18
                    color: "#333333"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Button {
                    id: personalSettingsButton
                    width: parent.width * 0.9
                    height: 40
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: parent.height * 0.2
                    anchors.horizontalCenter: parent.horizontalCenter
                    background: Rectangle
                    {
                        anchors.fill: parent
                        color: 'transparent'
                        radius: 2
                        border.color: "gray"
                        border.width: 0.5
                    }

                    Text{
                        text: qsTr("个人设置")
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 13
                    }
                }

                Button {
                    id: logOutButton
                    width: parent.width * 0.9
                    height: 40
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: parent.height * 0.05
                    anchors.horizontalCenter: parent.horizontalCenter
                    background: Rectangle
                    {
                        anchors.fill: parent
                        color: 'transparent'
                        radius: 2
                        border.color: "gray"
                        border.width: 0.5
                    }
                    onClicked: {
                        console.log("登出")
                        confirmLogoutDialog.open()
                    }
                    Text{
                        text: qsTr("登出")
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 13
                    }
                }
            }
        }

        // 右侧展示区
        Rectangle {
            id: displayArea
            color: "#ffffff"
            width: parent.width * 0.75 // 75% 的宽度用于展示区
            height: parent.height
            anchors.right: parent.right

            Column {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 10
                // 示例内容
                Rectangle {
                    width: parent.width
                    height: parent.height
                    color: "#d9edf7"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom

                    Text {
                        text: qsTr("这里是主要内容展示区")
                        anchors.centerIn: parent
                        font.pixelSize: 18
                        color: "#31708f"
                    }
                }
            }
        }
    }
    // 关闭按钮
    Rectangle {
        width: parent.height * 0.03
        height: parent.height * 0.03
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.01
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.01
        z: 1 // 确保关闭按钮在最上层
        color:"transparent"

        Image {
            id: closeDialogImage
            source: "qrc:/images/close.svg"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectFit
            rotation: closeDialogImage.rotation

            // 使用 Behavior 使旋转平滑
            Behavior on rotation {
                RotationAnimation {
                    duration: 300 // 动画持续时间为300ms
                    easing.type: Easing.InOutQuad
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                closeDialog.open()
            }
            // 鼠标悬停时触发旋转动画
            onHoveredChanged: {
                if (containsMouse) {
                    closeDialogImage.rotation = 180
                } else {
                    closeDialogImage.rotation = 0
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        z: -2 // 设置此 MouseArea 的 z 值为较低，确保它在文本和图标下方
        onPressed: {
            root.dragX = mouseX
            root.dragY = mouseY
            root.dragging = true
        }
        onReleased: root.dragging = false
        onPositionChanged: {
            if (root.dragging) {
                root.x += mouseX - root.dragX
                root.y += mouseY - root.dragY
            }
        }
    }
            // 弹出对话框
    Dialog {
        id: closeDialog
        title: "退出程序"
        modal: true
        width: 400
        height: 150
        standardButtons: Dialog.NoButton

        // 将对话框居中显示
        Component.onCompleted: {
            closeDialog.anchors.centerIn = parent
        }

        contentItem: Column {
            spacing: 15
            anchors.fill: parent
            anchors.margins: 20

            Text {
                id: closeDialogText
                text: "您想要关闭程序还是最小化到托盘？"
                wrapMode: Text.WordWrap
                font.pixelSize: 16
                color: "#333333"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: parent.height * 0.2
            }

            Row {
                spacing: 10
                anchors.top: closeDialogText.bottom
                anchors.topMargin: parent.height * 0.125
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    text: "关闭程序"
                    width: 100
                    height: 40
                    font.pixelSize: 14
                    background: Rectangle{
                        color: "#d9534f"
                    }
                    onClicked: {
                        Qt.quit() // 关闭程序
                    }
                }

                Button {
                    text: "最小化到托盘"
                    width: 100
                    height: 40
                    font.pixelSize: 14
                    background: Rectangle{
                        color: "#5bc0de"
                    }
                    onClicked: {
                        // 最小化到托盘关闭当前的窗口，并隐藏主窗口
                        root.close()
                        minimizeToTray()
                        closeDialog.close()
                    }
                }

                Button {
                    text: "取消"
                    width: 100
                    height: 40
                    font.pixelSize: 14
                    background: Rectangle{
                        color: "#f0ad4e"
                    }
                    onClicked: closeDialog.close()
                }
            }
        }
    }
    Dialog {
        id: confirmLogoutDialog
        title: qsTr("确认登出")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        contentItem: Column {
            spacing: 10
            Text {
                text: "您确定要登出吗？"
                wrapMode: Text.WordWrap
            }
        }
        // 将对话框居中显示
        Component.onCompleted: {
            confirmLogoutDialog.anchors.centerIn = parent
        }
        onAccepted: {
            console.log("用户确认登出")
            // 在这里实现登出的逻辑
            loader.sourceComponent = loginPage
        }

        onRejected: {
            console.log("用户取消登出")
            confirmLogoutDialog.close()
        }
    }
}
