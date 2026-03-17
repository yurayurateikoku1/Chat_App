import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    required property int uid
    required property string name
    required property string avatarSource

    signal signToolButnAgreeClicked(int uid)
    signal signToolButnDisAgreeClicked(int uid)
    implicitHeight: 56
    color: mouse_area.containsMouse ? Common.color1 : "white"
    radius: 4

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        hoverEnabled: true
    }

    RowLayout {
        anchors.fill: parent
        spacing: 10

        Rectangle {
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            color: Common.color3
            clip: true
            radius: 4
            Image {
                anchors.fill: parent
                source: root.avatarSource
                visible: root.avatarSource !== ""
                fillMode: Image.PreserveAspectCrop
            }

            Label {
                anchors.centerIn: parent
                text: root.name.charAt(0)
                font.pixelSize: 16
                color: "white"
                visible: root.avatarSource === ""
            }
        }

        Label {
            text: "uid:" + root.uid
            font.pixelSize: 14
        }

        Label {
            text: root.name
            font.pixelSize: 14
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButtonComp {
            id: toolbutton_agree
            Layout.preferredWidth: 30
            Layout.preferredHeight: 30
            Image {
                anchors.centerIn: parent
                height: 25
                width: 25
                source: "qrc:/assets/agree.png"
            }
            onClicked: root.signToolButnAgreeClicked(root.uid)
        }

        ToolButtonComp {
            id: toolbutton_disagree
            Layout.preferredWidth: 30
            Layout.preferredHeight: 30
            Image {
                anchors.centerIn: parent
                height: 25
                width: 25
                source: "qrc:/assets/disagree.png"
            }
            onClicked: root.signToolButnDisAgreeClicked(root.uid)
        }
    }
}
