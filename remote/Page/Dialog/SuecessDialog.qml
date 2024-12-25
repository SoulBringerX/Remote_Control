import QtQuick

Window {
    id: suecessDialog
    title: "SystemCall"
    width: Screen.desktopAvailableWidth * 0.25 // 设置窗口宽度为屏幕宽度的25%
    height: Screen.desktopAvailableHeight * 0.12 // 设置窗口高度为屏幕高度的12%
    color: "#00000000" // 设置透明背景
    flags: Qt.FramelessWindowHint // 去除状态栏和标题栏

    property bool registerstatus: true
    property int dragX: 0
    property int dragY: 0
    property bool dragging: false

    // 添加一个 message 变量
    property string message: "成功注册"

    // 背景
    Rectangle {
        width: parent.width
        height: parent.height
        color: "white"
        border.color: "#4CAF50"
        border.width: 0.5

        // 图标
        Rectangle {
            id: registerDialogIcon
            width: parent.height * 0.26 // 图标宽度为窗口高度的26%
            height: parent.height * 0.26 // 图标高度为窗口高度的26%
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.03 // 左侧间距为窗口宽度的3%
            anchors.verticalCenter: parent.verticalCenter

            Image {
                source: registerstatus ? "qrc:/images/OK.svg" : "qrc:/images/NO.svg"
                anchors.fill: parent
            }
        }

        // 文本
        Text {
            anchors.left: registerDialogIcon.right
            anchors.leftMargin: parent.width * 0.03 // 左侧间距为窗口宽度的3%
            anchors.verticalCenter: parent.verticalCenter // 垂直居中
            font.pointSize: parent.height * 0.15 // 根据窗口高度调整字体大小
            text: message
        }

        // 关闭按钮
        Rectangle {
            width: parent.height * 0.16
            height: parent.height * 0.16
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.05
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.01
            z: 1 // 确保关闭按钮在最上层

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
                onClicked: suecessDialog.visible = false

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

        // 可拖拽区域(无边框)
        MouseArea {
            anchors.fill: parent
            z: 0 // 设置此 MouseArea 的 z 值为较低，确保它在文本和图标下方
            onPressed: {
                registerDialog.dragX = mouseX
                registerDialog.dragY = mouseY
                registerDialog.dragging = true
            }
            onReleased: registerDialog.dragging = false
            onPositionChanged: {
                if (registerDialog.dragging) {
                    registerDialog.x += mouseX - registerDialog.dragX
                    registerDialog.y += mouseY - registerDialog.dragY
                }
            }
        }
    }
}
