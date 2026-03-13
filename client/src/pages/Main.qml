import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 400
    height: 600
    visible: true
    title: "Chat"
    StackView {
        id: stack_view
        anchors.fill: parent
        initialItem: LoginPageView {
            id: login_page
        }
    }

    Component {
        id: register_page
        RegisterPageView {}
    }

    Component {
        id: reset_page
        ResetPageView {}
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
