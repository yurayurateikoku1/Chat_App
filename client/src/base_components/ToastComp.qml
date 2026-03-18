import QtQuick
import QtQuick.Controls

Item {
    id: root
    property string text: ""
    property color textColor: "white"
    property int duration: 2200

    width: Math.min(contentText.implicitWidth + 24, 300)
    height: contentText.implicitHeight + 24
    visible: false
    opacity: 0
    z: 2000

    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
    y: 30 + yOffset
    property real yOffset: 0

    Rectangle {
        anchors.fill: parent
        radius: 10
        color: Common.color1
    }

    Text {
        id: contentText
        anchors.centerIn: parent
        text: root.text
        color: root.textColor
        font.pixelSize: 14
        font.bold: true
        wrapMode: Text.WrapAnywhere
        width: Math.min(280, implicitWidth)
        horizontalAlignment: Text.AlignHCenter
    }

    Behavior on opacity {
        NumberAnimation {
            duration: 220
            easing.type: Easing.OutCubic
        }
    }

    PropertyAnimation {
        id: enterAnim
        target: root
        property: "yOffset"
        from: -12
        to: 0
        duration: 220
        easing.type: Easing.OutCubic
    }

    PropertyAnimation {
        id: exitAnim
        target: root
        property: "yOffset"
        from: 0
        to: -12
        duration: 200
        easing.type: Easing.InCubic
        onStopped: root.visible = false
    }

    Timer {
        id: hideTimer
        interval: root.duration
        onTriggered: {
            root.opacity = 0;
            exitAnim.restart();
        }
    }

    function showMessage(msg, color) {
        root.text = msg;
        root.textColor = color;
        root.visible = true;
        root.opacity = 1;
        enterAnim.restart();
        hideTimer.restart();
    }
}
