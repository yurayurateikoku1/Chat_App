import QtQuick
import QtQuick.Controls

ButtonComp {
    id: root

    property int duration: 20
    property int remaining: 0
    property string originalText: ""

    enabled: remaining === 0

    Component.onCompleted: {
        originalText = text;
    }

    Timer {
        id: countdown
        interval: 1000
        repeat: true
        onTriggered: {
            root.remaining--;
            if (root.remaining <= 0) {
                countdown.stop();
                root.text = root.originalText;
            } else {
                root.text = qsTr("%1s").arg(root.remaining);
            }
        }
    }

    function startCountdown() {
        remaining = duration;
        text = qsTr("%1s").arg(remaining);
        countdown.start();
    }

    Connections {
        target: root
        function onClicked() {
            root.startCountdown();
        }
    }
}
