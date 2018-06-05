import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import communications 1.0

ApplicationWindow {
    visible: true
    width: 480
    height: 480
    title: qsTr("LuminT") 

    Rectangle {
        anchors.fill: parent
        focus: true

        Keys.onReleased: {
            if (event.key === Qt.Key_Back)
            {
                event.accepted = true
                window.pop(null)
            }
        }
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
        clip: false
        transformOrigin: Item.TopLeft
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
