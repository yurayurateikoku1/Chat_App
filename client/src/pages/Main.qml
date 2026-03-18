import QtQuick
import QtQuick.Controls
import QtQuick.Window

ApplicationWindow {
    id: main_window
    width: 400
    height: 600
    // width: 1024
    // height: 700
    visible: true
    title: "Chat"
    StackView {
        id: stack_view
        anchors.fill: parent
        initialItem: LoginPageView {
            id: login_page
        }
        // initialItem: ChatPageView {
        //     id: chat_page
        // }
    }

    Component {
        id: register_page
        RegisterPageView {}
    }

    Component {
        id: reset_page
        ResetPageView {}
    }

    Component {
        id: chat_page
        ChatPageView {}
    }

    Connections {
        target: RegisterPage
        function onSign2UIRegisterSuccess(email) {
            stack_view.pop();
            login_page.textfield_email.text = email;
            login_page.toast.showMessage(email + "Registration succeeded!", "green");
        }
    }

    Connections {
        target: ResetPage
        function onSign2UIResetSuccess(email) {
            stack_view.pop();
            login_page.textfield_email.text = email;
            login_page.toast.showMessage(email + "Reset password succeeded!", "green");
        }
    }

    Connections {
        target: LoginPage
        function onSign2UILoginStatus(status) {
            login_page.button_login.enabled = true;
            main_window.width = 1024;
            main_window.height = 700;
            main_window.x = (Screen.width - main_window.width) / 2;
            main_window.y = (Screen.height - main_window.height) / 2;
            stack_view.push(chat_page);
        }
    }

    Connections {
        target: stack_view.currentItem
        ignoreUnknownSignals: true
        function onSignButtonBackClicked() {
            stack_view.pop();
        }

        function onSignButtonRegisterClicked() {
            stack_view.push(register_page);
        }

        function onSignButtonForgetClicked() {
            stack_view.push(reset_page);
        }
    }
}
