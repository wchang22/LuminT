import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import communications 1.0

ApplicationWindow {
    visible: true
    width: 480
    height: 480
    title: qsTr("LuminT") 
    Component.onCompleted: {
        if (receiver.state() === Receiver.ERROR)
            errorDialog.open()
    }

    MessageDialog {
        id: errorDialog
        title: qsTr("Error")
        text: qsTr("Please make sure you are connected "+
                   "to internet before starting LuminT. Closing...")
        icon: StandardIcon.Critical
        onAccepted: Qt.quit()
        onVisibleChanged: if (!this.visible) Qt.quit()
    }

    Connections {
        target: sender
        onConnected: window.push(senderComp)
        onDisconnected: window.pop(null)
        onConnectionError: window.push(connectionErrorComp)
    }

    Connections {
        target: receiver
        onConnected: window.push(receiverComp)
        onDisconnected: window.pop(null)
        onConnectionError: window.push(connectionErrorComp)
    }

    header: ToolBar {
        ToolButton {
            text: qsTr("‹")
            onClicked: {
                sender.disconnectFromReceiver();
                receiver.stopServer();
                window.pop(null)
            }
        }
    }

    StackView {
        id: window
        anchors.fill: parent
        initialItem: startPageComp

        Component {
            id: startPageComp
            StartPage {}
        }

        Component {
            id: senderWaitingComp
            SenderWaitingPage {}
        }

        Component {
            id: receiverWaitingComp
            ReceiverWaitingPage {}
        }

        Component {
            id: registerComp
            RegisterPage {}
        }

        Component {
            id: senderComp
            SenderPage {}
        }

        Component {
            id: receiverComp
            ReceiverPage {}
        }

        Component {
            id: connectionErrorComp
            ConnectionErrorPage {}
        }
    }

}
