import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property alias text: text_edit.text
    radius: 6
    border.width: 1
    border.color: text_edit.activeFocus ? Common.color11 : Common.color0
    color: "white"

    TextEdit {
        id: text_edit
        anchors.fill: parent
        anchors.margins: 5
        wrapMode: TextEdit.Wrap
        font.pixelSize: 16
    }

    scale: text_edit.activeFocus ? 1.02 : 1.0
    Behavior on scale {
        NumberAnimation {
            duration: 100
        }
    }
}
