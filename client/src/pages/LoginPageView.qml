import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: login_page
    signal signButtonRegisterClicked
    width: 400
    height: 600
    ColumnLayout {
        anchors.centerIn: parent

        Image {
            Layout.alignment: Qt.AlignHCenter
            source: "qrc:/assets/chat.png"
            fillMode: Image.PreserveAspectFit
        }

        Item {
            Layout.fillHeight: true
        }

        TextFieldComp {
            id: textfield_username
            placeholderText: "Username"
            Layout.fillWidth: true
        }

        TextFieldComp {
            id: textfield_password
            placeholderText: "Password"
            Layout.fillWidth: true
        }

        ButtonComp {
            id: button_login
            text: "Login"
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.fillHeight: true
        }
        RowLayout {

            ButtonComp {
                id: button_register
                text: "Register"
                bgColor: Common.color3
                onClicked: login_page.signButtonRegisterClicked()
            }

            ButtonComp {
                id: button_forget
                text: "Forget"
                bgColor: Common.color3
            }
        }
    }
}
