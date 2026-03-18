import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Pane {
    id: root

    background: Item {}

    ColumnLayout {
        anchors.fill: parent

        //聊天标题
        ChatHeaderComp {
            id: chat_header
            Layout.fillWidth: true
            Layout.minimumHeight: 40
            Layout.maximumHeight: 40
            uid: ChatPage.getCurrentUid
            name: ChatPage.getCurrentName
            online: ChatPage.getOnLine(uid)
        }

        //信息窗列表
        MessageWindowList {
            id: messagewindow_list
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: ChatPage.getChatMessageModel
        }

        //中间按钮
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
        //文本编辑框
        TextEditComp {
            id: text_edit
            Layout.fillWidth: true
            Layout.preferredHeight: 150
        }
        //下方按钮区域
        RowLayout {
            Item {
                Layout.fillWidth: true
            }
            ButtonComp {
                id: button_send
                text: "Send"
                onClicked: {
                    ChatPage.sendMessage(chat_header.uid, text_edit.text);
                    text_edit.text = "";
                }
            }
        }
    }
}
