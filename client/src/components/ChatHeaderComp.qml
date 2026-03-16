import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property int uid: 0           //用户uid
    property string name: ""        //用户姓名
    property bool online: false     //用户是否在线

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 2
        Label {
            text: root.name
            font.pixelSize: 30
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            text: root.online ? "online" : "offline"
            font.pixelSize: 10
            color: root.online ? "green" : "gray"
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
