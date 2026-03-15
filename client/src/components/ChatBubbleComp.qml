import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string message: ""
    property string time: ""
    property bool isSelf: false
    property string avatarSource: ""

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
            color: root.isSelf ? Common.color6 : Common.color3
            clip: true

            Image {
                anchors.fill: parent
                source: root.avatarSource
                visible: root.avatarSource !== ""
                fillMode: Image.PreserveAspectCrop
                layer.enabled: true
                layer.effect: Item {
                    // 圆形裁剪通过父级 clip + radius 实现
                }
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
