import QtQuick
import QtQuick.Controls

Button {
    id: root
    implicitHeight: 35
    implicitWidth: 120
    property color bgColor: Common.color6
    property color hoverColor: Common.color3
    property color pressColor: Common.color0

    contentItem: Text {
        text: root.text
        font.pixelSize: 15
        font.bold: true
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        radius: 10
        color: root.pressed ? root.pressColor : root.hovered ? root.hoverColor : root.bgColor
    }

    scale: root.pressed ? 0.95 : 1.0
    Behavior on scale {
        NumberAnimation {
            duration: 100
        }
    }
}
