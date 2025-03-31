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
    property string filePath: ""
    property int fileTransferProgress: 0  // Progress of file transfer (0 to 100)

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
            enabled: filePath !== ""  // Only enabled if file is selected
            onClicked: {
                if(tcp.sendInstallPackage(filePath)){
                    showSucess("传输成功")
                }
                else {
                    showError("传输失败")
                }
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
                filePath = fileDialog.selectedFile.toString();
                filePath = filePath.replace("file:///", "");
                console.log("选中的文件路径:", filePath);

                if (!filePath.startsWith("/")) {
                    filePath = "/" + filePath;
                }

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

    function showError(message) {
        errorMessage = message;
        errorDialog.show();
    }

    function showSucess(message)
    {
        errorMessage = message
        registerDialog.show();
    }

    SuecessDialog{
        id:registerDialog
        message: filedialog.errorMessage
    }
    SystemErrorDialog{
        id:errorDialog
        message: filedialog.errorMessage
    }
}
