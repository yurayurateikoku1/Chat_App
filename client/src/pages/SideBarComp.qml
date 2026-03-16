import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Pane {
    id: root

    // 当前选中的按钮索引：0=头像, 1=聊天, 2=通讯录
    property int currentIndex: -1

    signal signToolButAvatarClicked
    signal signToolButChatClicked
    signal signToolButContactsClicked

    background: Item {}

    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        Item {
            Layout.minimumHeight: 80
            Layout.maximumHeight: 80
        }
        ToolButtonComp {
            id: toolbutton_avatar
            bgColor: root.currentIndex === 0 ? Common.color6 : "#FFFFFF"
            Image {
                source: "qrc:/assets/avatars/Artboard_1.png"
            }
            onClicked: {
                root.currentIndex = 0;
                root.signToolButAvatarClicked();
            }
        }
        ToolButtonComp {
            id: toolbutton_chat
            bgColor: root.currentIndex === 1 ? Common.color6 : "#FFFFFF"
            Image {
                anchors.centerIn: parent
                source: "qrc:/assets/duihuaxinxi.png"
                width: 32
                height: 32
            }
            onClicked: {
                root.currentIndex = 1;
                root.signToolButChatClicked();
            }
        }
        ToolButtonComp {
            id: toolbutton_Contacts
            bgColor: root.currentIndex === 2 ? Common.color6 : "#FFFFFF"
            Image {
                anchors.centerIn: parent
                source: "qrc:/assets/tongxunlu.png"
                width: 32
                height: 32
            }
            onClicked: {
                root.currentIndex = 2;
                root.signToolButContactsClicked();
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
