import QtQuick

Window {
    id: remoteView
    width: Screen.desktopAvailableWidth * 0.675
    height: Screen.desktopAvailableHeight * 0.675
    // 用于接收 FreeRDP 的图形数据
    Image {
            id: remoteImage
            anchors.fill: parent
            source: "image://remote/1" // 固定 ID，与提供器匹配
        }

    Connections {
        target: client
        onImageUpdated: {
            remoteImage.source = "";
            remoteImage.source = "image://remote/1?t=" + Date.now(); // 动态 URL
        }
    }
}
