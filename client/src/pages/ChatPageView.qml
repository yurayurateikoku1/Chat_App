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
        Pane {
            id: area0
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 5
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
                    }

                    ToolButtonComp {
                        id: toolbutton_search
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: 30
                        Image {
                            anchors.centerIn: parent
                            height: 25
                            width: 25
                            source: "qrc:/assets/sousuo.png"
                        }
                    }
                }

                CompoundList {
                    id: compound_list
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
        Pane {
            id: area1
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 15
            background: Item {}

            ColumnLayout {
                anchors.fill: parent

                ChatHeaderComp {
                    id: chat_header
                    Layout.fillWidth: true
                    Layout.minimumHeight: 40
                    Layout.maximumHeight: 40
                    name: ""
                    online: true
                }

                MessageWindowList {
                    id: messagewindow_list
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: ChatPage.chatMessageModel
                }

                RowLayout {
                    ToolButtonComp {
                        id: toolbutton_emoji
                        Layout.preferredWidth: 25
                        Layout.preferredHeight: 25
                        Image {
                            anchors.centerIn: parent
                            source: "qrc:/assets/kaixin.png"
                            width: 25
                            height: 25
                        }
                    }

                    ToolButtonComp {
                        id: toolbutton_upload
                        Layout.preferredWidth: 25
                        Layout.preferredHeight: 25
                        Image {
                            anchors.centerIn: parent
                            source: "qrc:/assets/shangchuan.png"
                            width: 25
                            height: 25
                        }
                    }

                    ToolButtonComp {
                        id: toolbutton_share
                        Layout.preferredWidth: 25
                        Layout.preferredHeight: 25
                        Image {
                            anchors.centerIn: parent
                            source: "qrc:/assets/fenxiang.png"
                            width: 25
                            height: 25
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }
                TextEditComp {
                    id: text_edit
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                }
                RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }
                    ButtonComp {
                        id: button_send
                        text: "Send"
                    }
                }
            }
        }
    }
}
