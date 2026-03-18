pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: root

    signal signContactClicked(int uid, string name, string icon, bool online)

    clip: true
    spacing: 2
    model: ChatPage.getContactListModel

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }

    delegate: ContactListItem {
        width: ListView.view.width
        onSignClicked: (uid, name, icon, online) => root.signContactClicked(uid, name, icon, online)
    }
}
