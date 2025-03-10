import QtQuick

Window{
    id: remoteView
    visible: false
    // 用于接收 FreeRDP 的图形数据
    Image {
            id: remoteImage
            anchors.fill: parent
            source: "image://remote/1" // 固定 ID，与提供器匹配
    }
    MouseArea {
        anchors.fill: parent
        // 允许同时监听左键和右键
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed: (mouse) => {
            const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height))
            if (point.x >= 0 && point.y >= 0) {
                if (mouse.button === Qt.LeftButton) {
                    // 左键按下：PTR_FLAGS_DOWN(0x8000) | PTR_FLAGS_BUTTON1(0x1000)
                    client.sendMouseEvent(
                        point.x, point.y,
                        0x8000 | 0x1000,
                        0x0000
                    )
                } else if (mouse.button === Qt.RightButton) {
                    // 右键按下：PTR_FLAGS_DOWN(0x8000) | PTR_FLAGS_BUTTON2(0x2000)
                    client.sendMouseEvent(
                        point.x, point.y,
                        0x8000 | 0x2000,
                        0x0000
                    )
                }
            }
        }

        onReleased: (mouse) => {
            const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height))
            if (point.x >= 0 && point.y >= 0) {
                if (mouse.button === Qt.LeftButton) {
                    // 左键释放：仅PTR_FLAGS_BUTTON1(0x1000)
                    client.sendMouseEvent(
                        point.x, point.y,
                        0x0000,
                        0x1000
                    )
                } else if (mouse.button === Qt.RightButton) {
                    // 右键释放：仅PTR_FLAGS_BUTTON2(0x2000)
                    client.sendMouseEvent(
                        point.x, point.y,
                        0x0000,
                        0x2000
                    )
                }
            }
        }

        onPositionChanged: (mouse) => {
            const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height))
            if (point.x >= 0 && point.y >= 0) {
                let flags = 0x0800; // PTR_FLAGS_MOVE，仅移动
                // 如果左键处于按下状态
                if (mouse.buttons & Qt.LeftButton) {
                    flags |= 0x8000 | 0x1000; // PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON1
                }
                // 如果右键处于按下状态
                if (mouse.buttons & Qt.RightButton) {
                    flags |= 0x8000 | 0x2000; // PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON2
                }
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
            if(visible)
            {
                // 强制刷新图像
                remoteImage.source = ""
                remoteImage.source = "image://remote/providerId?" + Math.random()

                // 自动缩放保持比例
                remoteImage.sourceSize.width = remoteView.width
                remoteImage.sourceSize.height = remoteView.height
                remoteImage.fillMode = Image.PreserveAspectFit
            }
        }
    }
    // 监听 client 的断开完成信号
    Connections {
        target: client
        onDisconnected: {
            console.log("远程连接已断开，开始停止远程线程")
            remoteControlThread.stopConnection()
            // 延迟销毁线程，确保线程退出后再销毁对象
            Qt.callLater(function() {
                remoteControlThread.destroy()
                remoteView.close()  // 关闭当前窗口
            })
        }
    }

    // 监听窗口关闭事件，先断开远程连接
    onClosing: {
        console.log("结束远程会话进程---时间：" + Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz ddd"))
    }
}
