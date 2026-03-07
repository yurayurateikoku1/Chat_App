import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 400
    height: 600
    visible: true

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

    Connections {
        target: stack_view.currentItem
        function onSignUIRegisterRequested() {
            stack_view.push(register_page);
        }
        function onSignUIBackRequested() {
            stack_view.pop();
        }
    }
}
