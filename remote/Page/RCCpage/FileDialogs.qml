import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1
import QtQuick.Dialogs
import "../Dialog/"

Window {
    id: filedialog
    width: 600
    height: 400
    visible: false
    title: "安装包选择"

    property string errorMessage: ""
    property string selectedFileInfo: ""
    property string deviceIP: ""  // 设备IP地址

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Text {
            text: "请选择安装包 (.exe)"
            font.pointSize: 24
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            text: "选择安装包"
            font.pointSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: fileDialog.open()
        }

        Rectangle {
            width: parent.width
            height: 100
            color: "#F0F0F0"
            radius: 10
            border.color: "#CCCCCC"
            visible: selectedFileInfo !== ""
            anchors.horizontalCenter: parent.horizontalCenter

            Flickable {
                width: parent.width
                height: parent.height
                contentWidth: textItem.width
                contentHeight: textItem.height

                Text {
                    id: textItem
                    text: "选中的安装包：" + selectedFileInfo
                    font.pointSize: 16
                    color: "#333333"
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent
                }
            }
        }

        Button {
            text: "传输安装包"
            font.pointSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: filePath !== ""  // 只有选择了文件时按钮可用
            onClicked: {
                // 在此处调用传输安装包的函数
                transferPackage(filePath);
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "选择安装包 (.exe)"
        currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
        nameFilters: ["Windows 安装包 (*.exe)", "所有文件 (*.*)"]
        fileMode: FileDialog.OpenFile

        onAccepted: {
            if (fileDialog.selectedFile) {
                let filePath = fileDialog.selectedFile.toString();
                filePath = filePath.replace("file:///", "");
                console.log("选中的文件路径:", filePath);

                if (!filePath.toLowerCase().endsWith(".exe")) {
                    showError("文件类型错误：请选择一个 .exe 安装包文件。");
                    return;
                }

                let packageInfo = user_device.getInstallPackageInfo(filePath);
                console.log("获取的安装包信息:", packageInfo);
                console.log("远程设备IP：", deviceIP);

                if (packageInfo && packageInfo.fileSize !== undefined && packageInfo.fileSize !== -1) {
                    selectedFileInfo = "文件路径: " + packageInfo.filePath + "\n" +
                                       "文件名称: " + packageInfo.fileName + "\n" +
                                       "文件大小: " + packageInfo.fileSize + " 字节";
                } else {
                    showError("无法获取文件信息，请重试！");
                }
            } else {
                showError("未选择任何文件！");
            }
        }
    }

    SystemErrorDialog {
        id: errorDialog
        message: filedialog.errorMessage
    }

    function showError(message) {
        errorMessage = message;
        errorDialog.message = message;
        errorDialog.show();
    }

    function transferPackage(fileInfo) {
        // 检查数据包是否为空
        if (!fileInfo || fileInfo.trim() === "") {
            showError("错误：数据包为空，无法传输！");
            return;
        }
        console.log("正在传输安装包的路径：", fileInfo);
        console.log("正在将安装包传输到设备IP：", deviceIP);
        // 这里加入传输安装包的相关代码

    }
}
