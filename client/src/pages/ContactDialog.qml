import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

//联系人详情弹窗
Dialog {
    id: root
    width: 300
    height: 300
    modal: true
    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    property int contactUid: -1
    property string contactName: ""
    property string contactAvatar: ""
    property bool contactOnline: false

    // 圆角无边框背景
    background: Rectangle {
        radius: 12
        color: "white"
    }

    // 半透明遮罩
    Overlay.modal: Rectangle {
        color: "#80000000"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10

        // 头像
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 64
            Layout.preferredHeight: 64
            radius: 32
            color: Common.color3
            clip: true

            Image {
                anchors.fill: parent
                source: root.contactAvatar
                visible: root.contactAvatar !== ""
                fillMode: Image.PreserveAspectCrop
            }

            Label {
                anchors.centerIn: parent
                text: root.contactName.charAt(0)
                font.pixelSize: 24
                color: "white"
                visible: root.contactAvatar === ""
            }
        }

        // 名称
        Label {
            text: root.contactName
            font.pixelSize: 18
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        // 在线状态
        Label {
            text: root.contactOnline ? "在线" : "离线"
            color: root.contactOnline ? "#4caf50" : "gray"
            font.pixelSize: 13
            Layout.alignment: Qt.AlignHCenter
        }

        // 发送消息按钮
        ButtonComp {
            text: "发送消息"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                ChatPage.startChat(root.contactUid);
                compound_list.showView(1);
                side_bar.currentIndex = 0;
                root.close();
            }
        }

        ButtonComp {
            text: "删除联系人"
            bgColor: "red"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                compound_list.showView(1);
                side_bar.currentIndex = 0;
                root.close();
            }
        }
    }
}
