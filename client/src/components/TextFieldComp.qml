import QtQuick
import QtQuick.Controls

TextField {
    id: root
    property int borderWidth: 1
    property color borderBgColor: Common.color3
    font.pixelSize: 14
    verticalAlignment: TextInput.AlignVCenter
    implicitHeight: 30
    rightPadding: root.text.length > 0 ? clear_btn.width + 8 : 12

    ToolButton {
        id: clear_btn
        width: 20
        height: 20
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.verticalCenter: parent.verticalCenter
        visible: root.text.length > 0
        contentItem: Text {
            text: "\u2715"
            font.pixelSize: 12
            color: "gray"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 10
            color: clear_btn.hovered ? Common.color0 : "transparent"
        }
        onClicked: root.clear()
    }

    background: Rectangle {
        radius: 10
        color: "white"
        border.color: parent.activeFocus ? Common.color11 : root.borderBgColor
        border.width: root.borderWidth
    }

    scale: root.activeFocus ? 1.02 : 1.0
    Behavior on scale {
        NumberAnimation {
            duration: 100
        }
    }
}
