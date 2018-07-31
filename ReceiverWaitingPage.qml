import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Page {
    id: receiverWaitingPage
    Component.onCompleted: receiver.startServer()

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Label {
            id: idLabel
            text: qsTr('ID: ' + receiver.getThisID())
            Layout.alignment: Qt.AlignHCenter
            font.bold: true
            font.pointSize: 20
        }

        Label {
            id: senderWaitingLabel
            text: qsTr('Waiting for Sender')
            Layout.alignment: Qt.AlignHCenter
            font.bold: true
            font.pointSize: 35
        }

        BusyIndicator {
            id: senderWaitingIndicator
            width: 150
            height: 150
            Layout.preferredHeight: 150
            Layout.preferredWidth: 150
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
    }
}
