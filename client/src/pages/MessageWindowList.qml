import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: root

    clip: true
    spacing: 5
    verticalLayoutDirection: ListView.TopToBottom

    delegate: ChatBubbleComp {
        width: ListView.view.width
    }

    onCountChanged: {
        positionViewAtEnd();
    }
}
