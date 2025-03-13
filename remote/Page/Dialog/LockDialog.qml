import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./"
// 设备安全锁配置对话框
Dialog {
   id: securityLockDialog
   title: "配置设备安全锁"
   modal: true
   width: 400
   height: 250
   background: Rectangle {
       color: "#ffffff"
       border.width: 1
       border.color: "#e0e0e0"
       radius: 8
   }

   ColumnLayout {
       anchors.fill: parent
       anchors.topMargin: 20
       spacing: 20

       TextField {
           id: newSPasswordField
           placeholderText: "设置安全锁密码"
           echoMode: TextInput.Password
           font.pixelSize: 16
           padding: 10
           background: Rectangle {
               color: "#f5f5f5"
               border.width: 1
               border.color: "#e0e0e0"
               radius: 4
           }
           width: parent.width * 0.8 // 统一输入框长度
       }

       TextField {
           id: confirmSPasswordField
           placeholderText: "确认安全锁密码"
           echoMode: TextInput.Password
           font.pixelSize: 16
           padding: 10
           background: Rectangle {
               color: "#f5f5f5"
               border.width: 1
               border.color: "#e0e0e0"
               radius: 4
           }
           width: parent.width * 0.8 // 统一输入框长度
       }

       RowLayout {
           anchors.bottom: parent.bottom
           anchors.bottomMargin: 20
           anchors.right: parent.right
           anchors.rightMargin: 20
           spacing: 10

           Button {
               text: "取消"
               font.pixelSize: 14
               onClicked: securityLockDialog.close()
               width: 80
               padding: 10
               background: Rectangle {
                   color: "#f5f5f5"
                   border.width: 1
                   border.color: "#e0e0e0"
                   radius: 4
               }
           }

           Button {
               text: "确定"
               font.pixelSize: 14
               onClicked: {
                   if (newPasswordField.text !== confirmPasswordField.text) {
                       console.log("安全锁密码和确认密码不一致");
                       errordialog.message = "安全锁密码和确认密码不一致"
                       errordialog.show()
                       return;
                   }
                   if (newPasswordField.text.length < 6) {
                       console.log("安全锁密码长度至少为6位");
                       errordialog.message = "安全锁密码长度至少为6位"
                       errordialog.show()
                       return;
                   }
                   console.log("设置设备安全锁密码: ", newPasswordField.text);
                   // 在这里调用你的 C++ 函数来设置设备安全锁密码
                   setSecurityLockPassword(newPasswordField.text);
                   securityLockDialog.close();
                   toggleSecurityLock(true); // 开启设备安全锁
               }
               width: 80
               padding: 10
               background: Rectangle {
                   color: "#0066ff"
                   border.width: 1
                   border.color: "#0066ff"
                   radius: 4
               }
               palette.buttonText: "#ffffff"
           }
       }
    }
   SystemErrorDialog{
    id:errordialog
   }
}
