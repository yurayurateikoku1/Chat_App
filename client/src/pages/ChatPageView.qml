import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: chat_page

    background: Rectangle {
        color: "#ffffff"
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        //侧边栏
        SideBarComp {
            id: side_bar
            Layout.preferredWidth: 80
            Layout.minimumWidth: 80
            Layout.maximumWidth: 80
            Layout.fillHeight: true
            onSignToolButChatClicked: compound_list.showView(1)
            onSignToolButContactsClicked: compound_list.showView(2)
            // onSignToolButAvatarClicked: contact_list.showView(0)
        }
        //搜索框和复合列表区域
        Pane {
            id: area0
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 6
            spacing: 2
            background: Item {}
            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Layout.fillWidth: true
                    TextFieldComp {
                        id: textfield_search
                        placeholderText: "Search contacts"
                        Layout.fillWidth: true
                        borderWidth: 1
                        borderBgColor: Common.color0
                        onActiveFocusChanged: {
                            if (activeFocus) {
                                compound_list.showView(0);
                                side_bar.currentIndex = -1;
                            }
                        }
                        onTextChanged: {
                            if (text.length > 0) {
                                compound_list.showView(0);
                                side_bar.currentIndex = -1;
                            }
                            ChatPage.searchContacts(text);
                        }
                    }

                    ToolButtonComp {
                        id: toolbutton_search
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: 30
                        Image {
                            anchors.centerIn: parent
                            height: 25
                            width: 25
                            source: "qrc:/assets/tianjiayonghu.png"
                        }
                        onClicked: adduser_dialog.open()
                    }
                }

                CompoundList {
                    id: compound_list
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onSignContactClicked: (uid, name, avatarSource, online) => {
                        contact_dialog.contactUid = uid;
                        contact_dialog.contactName = name;
                        contact_dialog.contactAvatar = avatarSource;
                        contact_dialog.contactOnline = online;
                        contact_dialog.open();
                    }
                }
            }
        }

        //聊天窗口区域
        StackView {
            id: area1
            Layout.fillWidth: true
            Layout.preferredWidth: 14
            Layout.fillHeight: true
            initialItem: ChatPageItem0 {}
        }
    }

    Connections {
        target: compound_list
        function onSignAddUserClicked() {
            adduser_dialog.open();
        }
    }

    ContactDialog {
        id: contact_dialog
    }

    AddUserDialog {
        id: adduser_dialog
    }
}
