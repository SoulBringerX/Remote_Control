import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: Screen.desktopAvailableWidth * 0.625 * 0.85 * 0.625
    height: Screen.desktopAvailableHeight * 0.675
    color: "transparent"

    // 软件列表视图
    ListView {
        id: listView
        anchors.fill: parent
        model: softwareManager ? softwareManager.softwareList : []  // 添加空值检查
        spacing: 5

        // 列表项委托
        delegate: Rectangle {
            width: listView.width  // 确保宽度与 ListView 一致
            height: 80
            color: index % 2 === 0 ? "#f0f0f0" : "white"

            Column {
                Text { text: "名称：" + modelData.name }
                Text { text: "版本：" + modelData.version }
                Text { text: "本机IP：" + modelData.localIPs.join(", ") }

                // 显示图标
                Image {
                    id: iconImage
                    source: modelData.iconPath
                    width: 32
                    height: 32
                    // 确保图标数据正确加载
                    Component.onCompleted: {
                        if (source === "") {
                            console.log("图标加载失败：" + modelData.name);
                        }
                    }
                }
            }

            // 分割线
            Rectangle {
                width: parent.width
                height: 1
                color: "#ddd"
                anchors.bottom: parent.bottom
            }
        }

        // 下拉刷新（可选）
        ScrollBar.vertical: ScrollBar {}
    }

    // 数据加载状态提示
    BusyIndicator {
        anchors.centerIn: parent
        running: softwareManager ? (softwareManager.softwareList.length === 0) : true
        visible: running
    }
}
