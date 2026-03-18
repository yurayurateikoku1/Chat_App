pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: root

    clip: true
    spacing: 2
    model: ChatPage.getChatListModel

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }

    delegate: ChatListItem {
        width: ListView.view.width
        selected: uid === ChatPage.getCurrentUid
        onSignClicked: ChatPage.switchChat(uid)
        onSignDeleteClicked: (uid) => ChatPage.getChatListModel.removeChat(uid)
    }

    // 滚动到底部时自动加载更多
    onAtYEndChanged: {
        if (atYEnd && root.model.hasMore && !root.model.loading) {
            root.model.loadMore();
        }
    }

    // 底部加载指示器
    footer: Item {
        width: ListView.view ? ListView.view.width : 0
        height: root.model.hasMore ? 40 : 0
        visible: root.model.hasMore

        BusyIndicator {
            anchors.centerIn: parent
            running: root.model.loading
        }
    }
}
