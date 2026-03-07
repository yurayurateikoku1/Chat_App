import QtQuick
import QtQuick.Controls

TextField {
    id: root
    font.pixelSize: 14
    verticalAlignment: TextInput.AlignVCenter
    implicitHeight: 30

    background: Rectangle {
        radius: 10
        color: "white"
        border.color: parent.activeFocus ? Common.color11 : Common.color3
        border.width: 1
    }

    scale: root.activeFocus ? 1.02 : 1.0
    Behavior on scale {
        NumberAnimation {
            duration: 100
        }
    }
}
