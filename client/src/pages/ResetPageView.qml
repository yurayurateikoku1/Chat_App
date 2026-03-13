import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: reset_page
    signal signButtonBackClicked
    width: 400
    height: 600

    ToastComp {
        id: toast
    }

    Connections {
        target: ResetPage
        function onSign2UIMessage(msg, normal) {
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

        RowLayout {
            TextFieldComp {
                id: textfield_verification
                placeholderText: "VerificationCode"
                Layout.fillWidth: true
            }

            TimerButtonComp {
                id: button_getverify
                text: "GetVerify"
                bgColor: Common.color3
                onClicked: ResetPage.getVerifyCode(textfield_email.text)
            }
        }

        RowLayout {
            TextFieldComp {
                id: textfield_password
                placeholderText: "NewPassword"
                echoMode: checkbox1.checked ? TextInput.Normal : TextInput.Password
                Layout.fillWidth: true
            }

            CheckBox {
                id: checkbox1
                Layout.alignment: Qt.AlignVCenter
                implicitWidth: 20
                implicitHeight: 20
                padding: 0
                indicator: Image {
                    anchors.centerIn: parent
                    width: 20
                    height: 20
                    source: checkbox1.checked ? "qrc:/assets/kejianxing-kejian.png" : "qrc:/assets/kejianxing-bukejian.png"
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        ButtonComp {
            id: button_reset
            text: "Reset"
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            onClicked: ResetPage.resetPassword(textfield_username.text, textfield_password.text, textfield_email.text, textfield_verification.text)
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
                onClicked: reset_page.signButtonBackClicked()
            }
        }
    }
}
