import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: senderWaitingPage
    objectName: "senderWaitingPage"
    Component.onCompleted: {
        sender.setup()
        sender.connectToReceiver()
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Label {
            id: label
            text: qsTr("Waiting for Receiver")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            font.bold: true
            font.pointSize: 40

        }

        BusyIndicator {
            id: busyIndicator
            width: 150
            height: 150
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.preferredHeight: 150
            Layout.preferredWidth: 150
        }


    }
}
