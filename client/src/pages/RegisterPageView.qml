import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: register_page
    signal signUIBackRequested
    width: 400
    height: 600

    ToastComp {
        id: toast
    }

    Connections {
        target: RegisterPage
        function onSignUIMessage(msg, normal) {
            toast.showMessage(msg, normal ? "green" : "red");
        }
    }

    ColumnLayout {
        anchors.centerIn: parent

        TextFieldComp {
            id: textfield_username
            placeholderText: "Username"
            Layout.fillWidth: true
        }

        TextFieldComp {
            id: textfield_email
            placeholderText: "Email"
            Layout.fillWidth: true
        }

        TextFieldComp {
            id: textfield_password
            placeholderText: "Password"
            Layout.fillWidth: true
        }

        TextFieldComp {
            id: textfield_confirmpd
            placeholderText: "ConfirmPassword"
            Layout.fillWidth: true
        }

        RowLayout {
            TextFieldComp {
                id: textfield_verification
                placeholderText: "verificationCode"
                Layout.fillWidth: true
            }

            ButtonComp {
                id: button_getverify
                text: "GetVerify"
                bgColor: Common.color3
                onClicked: RegisterPage.getVerifyCode(textfield_email.text)
            }
        }

        Item {
            Layout.fillHeight: true
        }

        ButtonComp {
            id: button_register
            text: "Register"
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            onClicked: toast.showMessage("错误提示", "red")
        }

        Item {
            Layout.fillHeight: true
        }
        RowLayout {

            ButtonComp {
                id: button_clear
                text: "Clear"
                Layout.fillWidth: true
                bgColor: Common.color3
            }

            ButtonComp {
                id: button_back
                Layout.fillWidth: true
                text: "Back"
                bgColor: Common.color3
                onClicked: register_page.signUIBackRequested()
            }
        }
    }
}
