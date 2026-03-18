import QtQuick
import QtQuick.Controls

ToolButton {
    id: root

    implicitWidth: 48
    implicitHeight: 48

    property color bgColor: "#FFFFFF"
    property color hoverColor: Common.color6
    property color pressColor: Common.color0

    contentItem: Text {
        text: root.text
        font.pixelSize: 18
        font.bold: true
        color: "black"

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
    }

    background: Rectangle {
        radius: 10
        color: root.pressed ? root.pressColor : root.hovered ? root.hoverColor : root.bgColor
    }

    scale: root.pressed ? 0.9 : 1.0

    Behavior on scale {
        NumberAnimation {
            duration: 100
        }
    }
}
