import QtQuick
import QtQuick.Controls

ListView {
    id: root

    clip: true
    spacing: 2
    model: ChatPage.getFriendRequestListModel

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }

    delegate: FriendRequestListItem {
        width: ListView.view.width
        onSignToolButnAgreeClicked: uid => {
            ChatPage.addUser2Contact(uid);
        }
        onSignToolButnDisAgreeClicked: uid => {
            ChatPage.clearFriendRequest(uid);
        }
    }
}
