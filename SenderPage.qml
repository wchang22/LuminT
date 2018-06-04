import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: senderPage
    padding: 30
    objectName: "senderPage"

    ColumnLayout {
        id: senderColumnLayout
        anchors.fill: parent

        ColumnLayout {
            id: sendTextColumnLayout
            spacing: 20
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                id: sendTextLabel
                text: qsTr("Send Text")
                font.family: "Segoe UI"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                font.bold: true
                font.pointSize: 20
            }

            TextArea {
                id: sendTextArea
                text: qsTr("")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                placeholderText: "Enter Text"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillWidth: true
                Layout.minimumHeight: 100
                horizontalAlignment: Text.AlignLeft
                selectByMouse: true
            }

            Row {
                id: sendTextButtonRow
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
                }

                Button {
                    id: sendButton
                    width: 100
                    text: qsTr("Send")
                }

                spacing: 5
            }
        }

        ColumnLayout {
            id: sendFileColumnLayout
            Layout.fillWidth: true
            spacing: 20
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            Label {
                id: sendFileLabel
                text: qsTr("Send File")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                font.bold: true
                font.pointSize: 20
            }

            Row {
                id: sendFileRow
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: false
                Label {
                    id: sendFileNameLabel
                    text: qsTr("File:")
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    id: sendFileNameField
                    width: 200
                    text: qsTr("")
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                Button {
                    id: sendFileBrowseButton
                    width: 75
                    text: qsTr("Browse")
                }

                Button {
                    id: sendFileButton
                    width: 75
                    text: qsTr("Send")
                }
                spacing: 10
            }

            Row {
                id: sendFileProgressRow
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Label {
                    id: sendFileProgressLabel
                    text: qsTr("Sending Progress:")
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                ProgressBar {
                    id: sendFileProgressBar
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
