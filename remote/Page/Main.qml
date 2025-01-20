import QtQuick
import QtQuick.Controls
import QtQuick.Window

import './RCCpage'
import './RCRpage'

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

    Component {
        id: registerPage
        Register {}
    }

    Component {
        id: mainWindowPage
        Mainwindow {}
    }

    Component {
        id: rcrmanWindowPage
        RCRMainwindow {}
    }
}
