import QtQuick
import QtQuick.Controls
import QtQuick.Window
ApplicationWindow {
    id: appWindow
    visible: false

    Loader {
        id: loader
        anchors.fill: parent
        // 初始加载主页面
        sourceComponent: loginPage
    }

    Component {
        id: loginPage
        Login {}
    }
    // 1.ssss
    Component {
        id: registerPage
        Register {}
    }

    Component {
        id: mainWindowPage
        Mainwindow {}
    }
}