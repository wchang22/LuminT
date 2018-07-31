import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import communications 1.0

Page {
    id: startPage
    title: qsTr("LuminT")

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Label {
            id: title
            text: qsTr("LuminT")
            Layout.preferredHeight: 70
            Layout.preferredWidth: 180
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            font.bold: true
            font.pointSize: 50
        }

        Label {
            id: label
            text: qsTr("Luminous Transfer")
            font.pointSize: 25
            font.italic: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Button {
            id: send
            text: qsTr("Send")
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
            Layout.rowSpan: 1
            Layout.preferredHeight: 40
            Layout.preferredWidth: 150
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            onClicked: window.push(registerComp)
        }

    }
}
