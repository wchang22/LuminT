import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import communications 1.0

ApplicationWindow {
    visible: true
    width: 480
    height: 480
    title: qsTr("LuminT") 

    Connections {
        target: sender
        onConnected: window.push(senderComp)
        onDisconnected: window.pop(null)
    }

    Connections {
        target: receiver
        onConnected: window.push(receiverComp)
        onDisconnected: window.pop(null)
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
    }


}
