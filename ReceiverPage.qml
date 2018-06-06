import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: receiverPage
    padding: 30
    objectName: "receiverPage"

    Connections {
        target: receiver
        onReceivedText: receiveTextArea.text = text
    }

    ColumnLayout {
        id: receiverColumnLayout
        anchors.fill: parent

        ColumnLayout {
            id: receiveTextColumnLayout
            spacing: 20
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                id: receiveTextLabel
                text: qsTr("Receive Text")
                font.family: "Segoe UI"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                font.bold: true
                font.pointSize: 20
            }

            TextArea {
                id: receiveTextArea
                text: qsTr("")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                placeholderText: "Wait for text"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillWidth: true
                Layout.minimumHeight: 100
                horizontalAlignment: Text.AlignLeft
                selectByMouse: true
                readOnly: true
            }

            Row {
                id: receiveTextButtonRow
                width: 200
                height: 50
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Button {
                    id: clipboardCopyButton
                    width: 100
                    text: qsTr("Copy")
                }

                Button {
                    id: clipboardPasteButton
                    width: 100
                    text: qsTr("Paste")
                }

                Button {
                    id: clearButton
                    width: 100
                    text: qsTr("Clear")
                    onClicked: receiveTextArea.text = ""
                }

                spacing: 5
            }
        }

        ColumnLayout {
            id: receiveFileColumnLayout
            Layout.fillWidth: true
            spacing: 20
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            Label {
                id: receiveFileLabel
                text: qsTr("Receive File")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                font.bold: true
                font.pointSize: 20
            }

            Row {
                id: receiveFileRow
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: false
                Label {
                    id: receiveFileNameLabel
                    text: qsTr("Destination:")
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    id: receiveFileNameField
                    width: 232
                    text: qsTr("")
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                Button {
                    id: receiveFileBrowseButton
                    width: 75
                    text: qsTr("Browse")
                }
                spacing: 10
            }

            Row {
                id: receiveFileProgressRow
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Label {
                    id: receiveFileProgressLabel
                    text: qsTr("Receiving Progress:")
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                ProgressBar {
                    id: receiveFileProgressBar
                    width: 250
                    height: 20
                    anchors.verticalCenter: parent.verticalCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    value: 0
                }
                spacing: 25
            }
        }

    }

}
