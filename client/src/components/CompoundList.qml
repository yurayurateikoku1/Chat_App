import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "white"
    clip: true

    // 0=添加用户, 1=聊天列表, 2=通讯录
    property int currentView: -1

    function showView(index) {
        currentView = index;
    }
    Item {
        anchors.fill: parent
        visible: root.currentView === 0
        ColumnLayout {
            ButtonComp {
                id: button_adduser
                text: "Add User"
                Layout.fillWidth: true
            }
            ListView {
                id: searchuesr_list
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    ListView {
        id: contact_list
        anchors.fill: parent
        anchors.margins: 5
        visible: root.currentView === 2
    }

    ListView {
        id: chat_list
        anchors.fill: parent
        anchors.margins: 5
        visible: root.currentView === 1
    }
}
