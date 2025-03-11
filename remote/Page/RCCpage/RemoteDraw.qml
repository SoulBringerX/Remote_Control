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

        // Monitor focus changes
        onActiveFocusChanged: {
            console.log("FocusScope active focus changed to: " + activeFocus)
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

            onPressed: (mouse) => {
                const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height))
                if (point.x >= 0 && point.y >= 0) {
                    if (mouse.button === Qt.LeftButton) {
                        client.sendMouseEvent(point.x, point.y, 0x8000 | 0x1000, 0x0000)
                    } else if (mouse.button === Qt.RightButton) {
                        client.sendMouseEvent(point.x, point.y, 0x8000 | 0x2000, 0x0000)
                    } else if (mouse.button === Qt.MiddleButton) {
                        client.sendMouseEvent(point.x, point.y, 0x8000 | 0x4000, 0x0000)
                    }
                }
            }

            onReleased: (mouse) => {
                const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height))
                if (point.x >= 0 && point.y >= 0) {
                    if (mouse.button === Qt.LeftButton) {
                        client.sendMouseEvent(point.x, point.y, 0x0000, 0x1000)
                    } else if (mouse.button === Qt.RightButton) {
                        client.sendMouseEvent(point.x, point.y, 0x0000, 0x2000)
                    } else if (mouse.button === Qt.MiddleButton) {
                        client.sendMouseEvent(point.x, point.y, 0x0000, 0x4000)
                    }
                }
            }

            onPositionChanged: (mouse) => {
                const point = client.convertToRemoteCoordinates(mouse.x, mouse.y, Qt.size(width, height))
                if (point.x >= 0 && point.y >= 0) {
                    let flags = 0x0800; // PTR_FLAGS_MOVE, only move
                    if (mouse.buttons & Qt.LeftButton) {
                        flags |= 0x8000 | 0x1000;
                    }
                    if (mouse.buttons & Qt.RightButton) {
                        flags |= 0x8000 | 0x2000;
                    }
                    if (mouse.buttons & Qt.MiddleButton) {
                        flags |= 0x8000 | 0x4000;
                    }
                    client.sendMouseEvent(point.x, point.y, flags, 0x0000)
                }
            }

            onClicked: {
                focusScope.forceActiveFocus()  // Force focus on click
                remoteView.requestActivate()   // Ensure window is active
                console.log("Mouse clicked, focus set to FocusScope, window activated")
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

    Connections {
        target: client
        function onDisconnected() {
            console.log("Remote connection disconnected, stopping thread")
            remoteControlThread.stopConnection()
            Qt.callLater(function() {
                remoteControlThread.destroy()
                remoteView.close()
            })
        }
    }

    onClosing: {
        console.log("Ending remote session --- Time: " + Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz ddd"))
    }

    Component.onCompleted: {
        focusScope.forceActiveFocus()  // Set focus on load
        remoteView.requestActivate()   // Activate window in OS
        console.log("Window loaded, focus set to FocusScope, window activated")
    }
}
