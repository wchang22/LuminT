import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: startPage
    objectName: "startPage"
    transformOrigin: Item.Center
    title: "LuminT"

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Label {
            id: title
            text: "LuminT"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            Layout.fillHeight: false
            Layout.fillWidth: false
            transformOrigin: Item.Center
            Layout.preferredHeight: 70
            Layout.preferredWidth: 180
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            font.italic: true
            font.bold: true
            font.pointSize: 50
        }

        Button {
            id: send
            text: qsTr("Send")
            focusPolicy: Qt.NoFocus
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredHeight: 40
            Layout.preferredWidth: 150
            onClicked: window.push(senderWaitingComp)
        }

        Button {
            id: receive
            text: qsTr("Receive")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.preferredHeight: 40
            Layout.preferredWidth: 150
            onClicked: window.push(receiverWaitingComp)
        }

        Button {
            id: register
            text: qsTr("Register Devices")
            Layout.preferredHeight: 40
            Layout.preferredWidth: 150
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            onClicked: window.push(registerComp)
        }
    }
}
