import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0

import utilities 1.0

ScrollView {
    Page {
        id: receiverPage
        padding: 30
        objectName: "receiverPage"

        FolderDialog {
            id: receiverFolderDialog
            title: qsTr("Select a folder")
            folder: StandardPaths.standardLocations(
                        StandardPaths.DocumentsLocation)[0]
            onAccepted: {
                var folderPath = receiverFolderDialog.folder.toString()
                folderPath = folderPath.replace(/^(file:\/{3})/,"")
                folderPath = decodeURIComponent(folderPath)
                receiveFolderNameField.text = folderPath

                receiver.setFilePath(folderPath)
            }
        }

        Connections {
            target: receiver
            onReceivedText: receiveTextArea.text = text
            onReceiveProgress: receiveFileProgressBar.value = progress
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
                        onClicked: utilities.copy(receiveTextArea.text)
                    }

                    Button {
                        id: clipboardPasteButton
                        width: 100
                        text: qsTr("Paste")
                        onClicked: receiveTextArea.text = utilities.paste()
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
                id: receiveFolderColumnLayout
                Layout.fillWidth: true
                spacing: 20
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                Label {
                    id: receiveFolderLabel
                    text: qsTr("Receive File")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    font.bold: true
                    font.pointSize: 20
                }

                Row {
                    id: receiveFolderRow
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillWidth: false
                    Label {
                        id: receiveFileNameLabel
                        text: qsTr("Destination:")
                        anchors.verticalCenter: parent.verticalCenter
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }

                    TextField {
                        id: receiveFolderNameField
                        width: 232
                        anchors.verticalCenter: parent.verticalCenter
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        readOnly: true
                        selectByMouse: true
                        Component.onCompleted: {
                            var documents = StandardPaths.standardLocations(
                                                StandardPaths.DocumentsLocation)[0]
                            documents = documents.replace(/^(file:\/{3})/,"")
                            documents = decodeURIComponent(documents)
                            receiveFolderNameField.text = documents
                        }
                    }

                    Button {
                        id: receiveFolderBrowseButton
                        width: 75
                        text: qsTr("Browse")
                        onClicked: receiverFolderDialog.open()
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
}
