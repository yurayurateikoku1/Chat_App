import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

//添加联系人弹窗（通过uid精确查找）
Dialog {
    id: root
    width: 400
    height: 350
    modal: true
    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    // 是否已点击搜索
    property bool searched: false

    onClosed: {
        uid_input.text = "";
        root.searched = false;
        ChatPage.clearFoundUser();
    }

    // 圆角无边框背景
    background: Rectangle {
        radius: 12
        color: "white"
    }

    // 半透明遮罩
    Overlay.modal: Rectangle {
        color: "#80000000"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // 搜索栏
        RowLayout {
            Layout.fillWidth: true
            TextFieldComp {
                id: uid_input
                placeholderText: "输入用户UID"
                Layout.fillWidth: true
                borderWidth: 1
                borderBgColor: Common.color0
            }

            ToolButtonComp {
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                Image {
                    anchors.centerIn: parent
                    height: 25
                    width: 25
                    source: "qrc:/assets/sousuo.png"
                }
                onClicked: {
                    var uid = parseInt(uid_input.text);
                    if (!isNaN(uid)) {
                        root.searched = true;
                        ChatPage.searchUser(uid);
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        // 找到用户 — 显示用户信息和添加按钮
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 10
            visible: root.searched && ChatPage.foundValid

            // 头像
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 64
                Layout.preferredHeight: 64
                radius: 32
                color: Common.color3
                clip: true

                Image {
                    anchors.fill: parent
                    source: ChatPage.foundAvatar
                    visible: ChatPage.foundAvatar !== ""
                    fillMode: Image.PreserveAspectCrop
                }

                Label {
                    anchors.centerIn: parent
                    text: ChatPage.foundName.charAt(0)
                    font.pixelSize: 24
                    color: "white"
                    visible: ChatPage.foundAvatar === ""
                }
            }

            // 名称
            Label {
                text: ChatPage.foundName
                font.pixelSize: 18
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            // 添加联系人按钮
            ButtonComp {
                text: "添加联系人"
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    ChatPage.addUser2Contact(ChatPage.foundUid);
                    root.close();
                }
            }

            ButtonComp {
                text: "关闭"
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    root.close();
                }
            }
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 10
            visible: root.searched && !ChatPage.foundValid

            // 未找到用户
            Label {
                text: "未找到用户"
                font.pixelSize: 16
                color: "gray"
                Layout.alignment: Qt.AlignHCenter
            }

            ButtonComp {
                text: "关闭"
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    root.close();
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
