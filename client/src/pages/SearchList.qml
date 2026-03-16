pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: root

    signal signContactClicked(int uid, string name, string avatarSource, bool online)

    clip: true
    spacing: 2
    model: ChatPage.getSearchListModel

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }

    delegate: ContactListItem {
        width: ListView.view.width
        onSignClicked: (uid, name, avatarSource, online) => root.signContactClicked(uid, name, avatarSource, online)
    }
}
