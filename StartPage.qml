import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: startPage
    objectName: "startPage"
    transformOrigin: Item.Center
    anchors.fill: parent
    title: "LuminT"

    Label {
        id: title
        x: 242
        text: "LuminT"
        anchors.verticalCenterOffset: -100
        anchors.horizontalCenterOffset: 0
        anchors.centerIn: parent
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        font.italic: true
        font.bold: true
        font.pointSize: 50
    }

    ColumnLayout {
        id: column
        width: 200
        anchors.topMargin: 227
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 0

        Button {
            id: send
            width: 150
            height: 50
            text: qsTr("Send")
            focusPolicy: Qt.NoFocus
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.preferredHeight: 40
            Layout.preferredWidth: 150
            anchors.verticalCenterOffset: -40
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: window.push(senderWaitingComp)
        }

        Button {
            id: receive
            width: 150
            height: 50
            text: qsTr("Receive")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.preferredHeight: 40
            Layout.preferredWidth: 150
            anchors.verticalCenterOffset: 40
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: window.push(receiverWaitingComp)
        }

    }
}
