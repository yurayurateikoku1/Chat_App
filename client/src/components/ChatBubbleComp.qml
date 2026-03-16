import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    required property string message
    required property string time
    required property bool isSelf

    // 头像：自己的为空（显示占位），对方的从联系人模型按当前会话uid查询
    readonly property string avatarSource: isSelf ? "" : ChatPage.getAvatar(ChatPage.getCurrentUid)

    implicitHeight: bubble_layout.implicitHeight + 10

    RowLayout {
        id: bubble_layout
        anchors.left: root.isSelf ? undefined : parent.left
        anchors.right: root.isSelf ? parent.right : undefined
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        layoutDirection: root.isSelf ? Qt.RightToLeft : Qt.LeftToRight
        spacing: 8

        Rectangle {
            id: avatar
            width: 36
            height: 36
            radius: 18
            color: root.avatarSource !== "" ? "transparent" : (root.isSelf ? Common.color6 : Common.color3)
            clip: true

            Image {
                anchors.fill: parent
                source: root.avatarSource
                visible: root.avatarSource !== ""
                fillMode: Image.PreserveAspectCrop
            }

            Label {
                anchors.centerIn: parent
                text: root.isSelf ? "Me" : "Ta"
                font.pixelSize: 12
                color: "white"
                visible: root.avatarSource === ""
            }
        }

        Rectangle {
            id: bubble
            radius: 10
            color: root.isSelf ? Common.color6 : Common.color1
            implicitWidth: Math.min(msg_text.implicitWidth + 24, root.width * 0.6)
            implicitHeight: msg_layout.implicitHeight + 16

            ColumnLayout {
                id: msg_layout
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Label {
                    id: msg_text
                    text: root.message
                    font.pixelSize: 14
                    color: root.isSelf ? "white" : "black"
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                    Layout.maximumWidth: root.width * 0.6 - 24
                }

                Label {
                    text: root.time
                    font.pixelSize: 9
                    color: root.isSelf ? "#ccffffff" : "gray"
                    Layout.alignment: root.isSelf ? Qt.AlignLeft : Qt.AlignRight
                }
            }
        }
    }
}
