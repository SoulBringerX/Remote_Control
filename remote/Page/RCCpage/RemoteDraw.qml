import QtQuick 2.15

Window {
    id: remoteView
    width: 1280
    height: 720
    visible: false // Set to true if the window should be visible by default
    title: "Remote Desktop"

    // Display FreeRDP graphical data
    Image {
        id: remoteImage
        anchors.fill: parent
        source: "image://remote/1"
    }

    // Manage focus with FocusScope
    FocusScope {
        id: focusScope
        anchors.fill: parent
        focus: true
        Component.onCompleted: focusScope.forceActiveFocus()

        // Monitor focus changes
        onActiveFocusChanged: {
            console.log("FocusScope active focus changed to: " + activeFocus)
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
            hoverEnabled: true

            onPressed: (mouse) => {
                const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height));
                if (point.x >= 0 && point.y >= 0) {
                    let flags = 0x8000; // PTR_FLAGS_DOWN
                    if (mouse.button === Qt.LeftButton) {
                        flags |= 0x1000; // PTR_FLAGS_BUTTON1 (左键)
                    } else if (mouse.button === Qt.RightButton) {
                        flags |= 0x2000; // PTR_FLAGS_BUTTON2 (右键)
                    } else if (mouse.button === Qt.MiddleButton) {
                        flags |= 0x4000; // PTR_FLAGS_BUTTON3 (中键点击)
                    }
                    client.sendMouseEvent(point.x, point.y, flags, 0x0000);
                    console.log("Mouse Pressed at: ", point.x, point.y, "Flags:", flags);
                }
            }

            onReleased: (mouse) => {
                const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height));
                if (point.x >= 0 && point.y >= 0) {
                    let releaseFlags = 0x0000;
                    if (mouse.button === Qt.LeftButton) {
                        releaseFlags = 0x1000; // 释放左键
                    } else if (mouse.button === Qt.RightButton) {
                        releaseFlags = 0x2000; // 释放右键
                    } else if (mouse.button === Qt.MiddleButton) {
                        releaseFlags = 0x4000; // 释放中键
                    }
                    client.sendMouseEvent(point.x, point.y, 0x0000, releaseFlags);
                    console.log("Mouse Released at: ", point.x, point.y, "Release Flags:", releaseFlags);
                }
            }

            onPositionChanged: (mouse) => {
                const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height));
                if (point.x >= 0 && point.y >= 0) {
                    let flags = 0x0800; // PTR_FLAGS_MOVE
                    if (mouse.buttons & Qt.LeftButton) {
                        flags |= 0x8000 | 0x1000; // 按住左键移动
                    }
                    if (mouse.buttons & Qt.RightButton) {
                        flags |= 0x8000 | 0x2000; // 按住右键移动
                    }
                    if (mouse.buttons & Qt.MiddleButton) {
                        flags |= 0x8000 | 0x4000; // 按住中键移动
                    }
                    client.sendMouseEvent(point.x, point.y, flags, 0x0000);
                }
            }

            onClicked: {
                focusScope.forceActiveFocus();
                remoteView.requestActivate();
                console.log("Mouse clicked, focus set to FocusScope, window activated");
            }

            // 监听滚轮事件
            WheelHandler {
                id: wheelHandler
                onWheel: (wheel) => {
                    console.log("Wheel detected in QML:", wheel.angleDelta.y);
                    client.handleWheelEvent(wheel, widgetSize); // 传递大小
                }
            }
        }

        Keys.onPressed: (event) => {
            var rdpKey = client.convertQtKeyToRdpKey(event.key, event.text, event.nativeScanCode);
            client.sendKeyboardEvent(true, rdpKey, false);
            event.accepted = true;
        }

        Keys.onReleased: (event) => {
            var rdpKey = client.convertQtKeyToRdpKey(event.key, event.text, event.nativeScanCode);
            client.sendKeyboardEvent(false, rdpKey, false);
            event.accepted = true;
        }

    }

    Connections {
        target: client
        function onImageUpdated() {
            if (visible) {
                // Refresh image directly
                remoteImage.source = "image://remote/providerId?" + Math.random()
                remoteImage.sourceSize.width = remoteView.width
                remoteImage.sourceSize.height = remoteView.height
                remoteImage.fillMode = Image.PreserveAspectFit
            }
        }
    }

    // 在窗口关闭时通知线程停止并释放资源
    onClosing: {
        console.log("Ending remote session --- Time: " + Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz ddd"))
        remoteControlThread.stopConnection()
    }


    Component.onCompleted: {
        // focusScope.forceActiveFocus()  // Set focus on load
        remoteView.requestActivate()   // Activate window in OS
        console.log("Window loaded, focus set to FocusScope, window activated")
    }
}
