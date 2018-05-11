import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: receiverWaitingPage
    objectName: "receiverWaitingPage"
    anchors.fill: parent
    Component.onCompleted: receiver.startServer()

    Label {
        id: label1
        text: qsTr("Waiting for Sender")
        anchors.verticalCenterOffset: -100
        anchors.horizontalCenterOffset: 0
        font.bold: true
        font.pointSize: 40
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    BusyIndicator {
        id: busyIndicator1
        width: 150
        height: 150
        anchors.verticalCenterOffset: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}
