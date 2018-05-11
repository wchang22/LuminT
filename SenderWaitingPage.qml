import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: senderWaitingPage
    objectName: "senderWaitingPage"
    anchors.fill: parent
    Component.onCompleted: {
        sender.setup()
        sender.connectToReceiver()
    }

    Label {
        id: label
        text: qsTr("Waiting for Receiver")
        anchors.verticalCenterOffset: -100
        anchors.horizontalCenterOffset: 0
        font.bold: true
        font.pointSize: 40
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    BusyIndicator {
        id: busyIndicator
        width: 150
        height: 150
        anchors.verticalCenterOffset: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}
