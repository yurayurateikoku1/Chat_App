import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    required property int uid
    required property string name
    required property string icon
    required property bool online

    signal signClicked(int uid, string name, string icon, bool online)

    implicitHeight: 56
    color: mouse_area.containsMouse ? Common.color1 : "white"
    radius: 4

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.signClicked(root.uid, root.name, root.icon, root.online)
    }

    RowLayout {
        anchors.fill: parent
        spacing: 10

        // 头像（带在线状态指示）
        Item {
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            Layout.alignment: Qt.AlignVCenter

            Rectangle {
                width: 48
                height: 48
                color: Common.color3
                // clip: true
                radius: 4
                Image {
                    width: 48
                    height: 48
                    anchors.centerIn: parent
                    source: root.icon
                    visible: root.icon !== ""
                    fillMode: Image.PreserveAspectCrop
                }

                Label {
                    anchors.centerIn: parent
                    text: root.name.charAt(0)
                    font.pixelSize: 16
                    color: "white"
                    visible: root.icon === ""
                }
            }

            // 在线状态小圆点
            Rectangle {
                width: 10
                height: 10
                radius: 5
                color: root.online ? "#4caf50" : "gray"
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                border.width: 2
                border.color: "white"
            }
        }

        // 名称
        Label {
            text: root.name
            font.pixelSize: 15
            elide: Text.ElideRight
            Layout.fillWidth: true
        }

        // 在线状态文字
        Label {
            text: root.online ? "在线" : "离线"
            font.pixelSize: 11
            color: root.online ? "#4caf50" : "gray"
        }
    }
}
