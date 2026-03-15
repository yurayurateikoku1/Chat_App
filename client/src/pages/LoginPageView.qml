import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: login_page
    signal signButtonRegisterClicked
    signal signButtonForgetClicked
    property alias textfield_email: textfield_email
    property alias toast: toast
    property alias button_login: button_login
    width: 400
    height: 600

    ToastComp {
        id: toast
    }

    Connections {
        target: LoginPage
        function onSign2UIMessage(msg, normal) {
            toast.showMessage(msg, normal ? "green" : "red");
        }
    }

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
            id: textfield_email
            placeholderText: "Email"
            Layout.fillWidth: true
        }

        RowLayout {
            TextFieldComp {
                id: textfield_password
                placeholderText: "Password"
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

        ButtonComp {
            id: button_login
            text: "Login"
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                button_login.enabled = false;
                LoginPage.loginUser(textfield_email.text, textfield_password.text);
            }
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
                onClicked: login_page.signButtonForgetClicked()
            }
        }
    }
}
