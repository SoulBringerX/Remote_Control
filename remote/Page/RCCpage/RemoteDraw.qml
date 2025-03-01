import QtQuick

Window{
    id: remoteView
    // 用于接收 FreeRDP 的图形数据
    Image {
            id: remoteImage
            anchors.fill: parent
            source: "image://remote/1" // 固定 ID，与提供器匹配
    }
    MouseArea {
        anchors.fill: parent
        onPressed: (mouse) => {
            const point = client.convertToRemoteCoordinates(mouse.x, mouse.y)
            if (point.x >= 0 && point.y >= 0) {
                // 左键按下：PTR_FLAGS_DOWN(0x0001) | PTR_FLAGS_BUTTON1(0x1000)
                client.sendMouseEvent(
                    point.x, point.y,
                    0x0001 | 0x1000, // 组合标志
                    0x0000           // 不立即释放
                )
            }
        }
        onReleased: (mouse) => {
            const point = client.convertToRemoteCoordinates(mouse.x, mouse.y)
            if (point.x >= 0 && point.y >= 0) {
                // 左键释放：仅PTR_FLAGS_BUTTON1(0x1000)
                client.sendMouseEvent(
                    point.x, point.y,
                    0x0000,         // 无按下事件
                    0x1000           // 释放标志
                )
            }
        }
        onPositionChanged: (mouse) => {
            const point = client.convertToRemoteCoordinates(mouse.x, mouse.y)
            if (point.x >= 0 && point.y >= 0) {
                // 移动事件处理
                const flags = mouse.buttons & Qt.LeftButton ?
                    (0x0001 | 0x1000 | 0x0800) : // 拖拽时保持按下状态
                    0x0800;                       // 纯移动

                client.sendMouseEvent(
                    point.x, point.y,
                    flags,
                    0x0000
                )
            }
        }
    }

    Connections {
        target: client
        onImageUpdated: {
            // 强制刷新图像
            remoteImage.source = ""
            remoteImage.source = "image://remote/providerId?" + Math.random()

            // 自动缩放保持比例
            remoteImage.sourceSize.width = remoteView.width
            remoteImage.sourceSize.height = remoteView.height
            remoteImage.fillMode = Image.PreserveAspectFit
        }
    }
    // 监听窗口关闭事件
    onClosing: {
        // 退出远程桌面且销毁远程会话线程
        console.log("结束远程会话进程---时间："+Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz ddd"))
        // client.disconnect()
    }
}
