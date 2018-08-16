import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import communications 1.0
import qml 1.0

ApplicationWindow {
    visible: true
    width: (Qt.platform.os == 'android') ? Screen.width : 480
    height: (Qt.platform.os == 'android') ? Screen.height : Screen.height * 0.95
    title: qsTr('LuminT')
    onClosing: close.accepted = true
    property int appState: Qt.application.state

    Component.onCompleted: {
        if (receiver.state() === Receiver.ServerState.ERROR)
            errorPopup.open();
    }

    onAppStateChanged: {
        if (Qt.platform.os != 'android' || appState !== Qt.ApplicationSuspended)
            return;

        registerDeviceList.writeDeviceItems();

        if (sender.getMessageState() === Sender.MessageState.FILE_PAUSED ||
            sender.getMessageState() === Sender.MessageState.FILE_SENDING)
            sender.saveFileTransferInfo();

        if (receiver.getMessageState() === Receiver.MessageState.FILE_PAUSED ||
            receiver.getMessageState() === Receiver.MessageState.FILE_SENDING)
            receiver.saveFileTransferInfo();
    }

    Popup {
        id: errorPopup
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 300
        height: 150
        modal: true
        closePolicy: Popup.NoAutoClose
        ColumnLayout {
            anchors.fill: parent
            Label {
                text: qsTr('Please make sure you are connected\n'+
                           'to internet before starting LuminT.\nClosing...')
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            }
            Button {
                text: qsTr('Ok')
                Layout.preferredWidth: 75
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                onClicked: {
                    errorPopup.close();
                    Qt.quit();
                }
            }
        }
    }

    Connections {
        target: sender
        onConnected: window.push(senderComp)
        onDisconnected: window.pop(null)
        onConnectionError: window.push(connectionErrorComp)
        onConnectionUnrecognized: window.push(connectionUnrecognizedComp)
    }

    Connections {
        target: receiver
        onConnected: window.push(receiverComp)
        onDisconnected: window.pop(null)
        onConnectionError: window.push(connectionErrorComp)
        onConnectionUnrecognized: window.push(connectionUnrecognizedComp)
    }

    header: ToolBar {
        ToolButton {
            id: backButton
            text: qsTr('‹')
            onClicked: {
                sender.disconnectFromReceiver();
                receiver.stopServer();
                window.pop(null);
            }
        }
    }

    StackView {
        id: window
        anchors.fill: parent
        initialItem: startPageComp
        focus: true
        Keys.onReleased:
        {
            if (event.key !== Qt.Key_Back)
                return;

            if (window.depth === 1)
                Qt.quit();

            sender.disconnectFromReceiver();
            receiver.stopServer();
            window.pop(null);

            event.accepted = true;
        }

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
            id: connectionUnrecognizedComp
            ConnectionUnrecognizedPage {}
        }

        Component {
            id: connectionErrorComp
            ConnectionErrorPage {}
        }
    }

}
