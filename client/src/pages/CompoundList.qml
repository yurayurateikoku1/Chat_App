import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "white"
    clip: true

    // 0=添加用户, 1=聊天列表, 2=通讯录
    property int currentView: -1

    signal signContactClicked(int uid, string name, string avatarSource, bool online)
    signal signAddUserClicked

    function showView(index) {
        currentView = index;
    }
    Item {
        anchors.fill: parent
        visible: root.currentView === 0
        ColumnLayout {
            anchors.fill: parent
            ButtonComp {
                id: button_adduser
                text: "Add User"
                Layout.fillWidth: true
                onClicked: root.signAddUserClicked()
            }
            SearchList {
                id: searchuesr_list
                Layout.fillWidth: true
                Layout.fillHeight: true
                onSignContactClicked: (uid, name, avatarSource, online) => root.signContactClicked(uid, name, avatarSource, online)
            }
        }
    }

    ContactList {
        id: contact_list
        anchors.fill: parent
        anchors.margins: 5
        visible: root.currentView === 2
        onSignContactClicked: (uid, name, avatarSource, online) => root.signContactClicked(uid, name, avatarSource, online)
    }

    ChatList {
        id: chat_list
        anchors.fill: parent
        anchors.margins: 5
        visible: root.currentView === 1
    }
}
