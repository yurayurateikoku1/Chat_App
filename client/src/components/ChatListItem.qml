import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    required property int uid
    required property string name
    required property string lastMessage
    required property string time
    required property string avatarSource
    required property int unreadCount
    signal signClicked
    signal signDeleteClicked(int uid)

    property bool selected: false
    property bool showDelete: false

    implicitHeight: 68
    color: "transparent"
    radius: 6
    clip: true

    onActiveFocusChanged: {
        if (!activeFocus)
            showDelete = false;
    }

    // 删除区域（右侧1/3）
    Rectangle {
        id: deleteArea
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width / 3
        color: "#ff4444"
        radius: 6
        visible: root.showDelete

        Label {
            anchors.centerIn: parent
            text: "删除"
            color: "white"
            font.pixelSize: 14
            font.bold: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.signDeleteClicked(root.uid)
        }
    }

    // 内容区域
    Rectangle {
        id: contentArea
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: root.showDelete ? parent.width * 2 / 3 : parent.width
        color: root.selected ? Common.color1 : (mouse_area.containsMouse ? Common.color1 : "white")
        radius: 6

        Behavior on width {
            NumberAnimation { duration: 150 }
        }

        MouseArea {
            id: mouse_area
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: (mouse) => {
                if (mouse.button === Qt.RightButton) {
                    root.showDelete = true;
                    root.forceActiveFocus();
                } else {
                    root.showDelete = false;
                    root.signClicked();
                }
            }
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: 2
            spacing: 10

            // 头像
            Rectangle {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                radius: 22
                color: Common.color3
                clip: true

                Image {
                    anchors.fill: parent
                    source: root.avatarSource
                    visible: root.avatarSource !== ""
                    fillMode: Image.PreserveAspectCrop
                }

                Label {
                    anchors.centerIn: parent
                    text: root.name.charAt(0)
                    font.pixelSize: 18
                    color: "white"
                    visible: root.avatarSource === ""
                }
            }

            // 名称和最后一条消息
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: root.name
                        font.pixelSize: 15
                        font.bold: true
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    Label {
                        text: root.time
                        font.pixelSize: 11
                        color: "gray"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: root.lastMessage
                        font.pixelSize: 13
                        color: "gray"
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    // 未读消息红点
                    Rectangle {
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        radius: 10
                        color: "#ff4444"
                        visible: root.unreadCount > 0

                        Label {
                            anchors.centerIn: parent
                            text: root.unreadCount > 99 ? "99+" : root.unreadCount
                            font.pixelSize: 10
                            color: "white"
                        }
                    }
                }
            }
        }
    }
}
