import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: receiverWaitingPage
    objectName: "receiverWaitingPage"
    Component.onCompleted: receiver.startServer()

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Label {
            id: label1
            text: qsTr("Waiting for Sender")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            font.bold: true
            font.pointSize: 40
        }

        BusyIndicator {
            id: busyIndicator1
            width: 150
            height: 150
            Layout.preferredHeight: 150
            Layout.preferredWidth: 150
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }


    }
}
