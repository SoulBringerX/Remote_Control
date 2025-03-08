import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Popup {
    id: root
    width: Math.min(parent.width * 0.8, 300)
    height: 48
    x: (parent.width - width) / 2
    y: 20
    closePolicy: Popup.NoAutoClose
    padding: 0

    property alias text: label.text
    property color backgroundColor: "#2ecc71"

    background: Rectangle {
        radius: 4
        color: root.backgroundColor
        layer.enabled: true
        layer.effect: DropShadow {
            radius: 8
            samples: 16
            color: "#40000000"
        }
    }

    Label {
        id: label
        anchors.centerIn: parent
        color: "white"
        font.pixelSize: 14
    }

    Timer {
        interval: 2000
        running: true
        onTriggered: root.close()
    }

    function show() {
        open()
        timer.restart()
    }
}
