import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: root

    clip: true
    spacing: 5
    verticalLayoutDirection: ListView.BottomToTop

    delegate: ChatBubbleComp {
        required property string message
        required property string time
        required property bool isSelf
        required property string avatarSource
        width: ListView.view.width
    }

    onCountChanged: {
        positionViewAtEnd()
    }
}
