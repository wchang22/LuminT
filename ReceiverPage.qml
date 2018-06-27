import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0
import QtQuick.Dialogs 1.2

import utilities 1.0
import communications 1.0

ScrollView {
    Page {
        id: receiverPage
        padding: 30
        objectName: "receiverPage"

        Connections {
            target: receiver
            onReceivedText: receiveTextArea.text = text
            onReceiveProgress:
            {
                receiveFileProgressBar.value = progress
                if (receiveFilePauseButton.text == "Resume")
                    receiveFilePauseButton.text = "Pause"
            }
            onFileStatus: {
                switch(status)
                {
                    case Receiver.FileState.EXISTS:
                        fileExistsField.text = fileName
                        fileExistsPopup.open()
                        break
                    case Receiver.FileState.ERROR:
                        fileErrorPopup.open()
                        break
                    case Receiver.FileState.OK:
                        receiver.requestFirstPacket()
                }
            }
            onFilePaused: receiveFilePauseButton.text = "Resume"
            onFileResumed: receiveFilePauseButton.text = "Pause"
        }

        FolderDialog {
            id: receiveFileDialog
            title: qsTr("Select a folder")
            folder: StandardPaths.standardLocations(
                        StandardPaths.DocumentsLocation)[0]
            onAccepted: {
                var folderPath = receiveFileDialog.folder.toString()
                folderPath = folderPath.replace(/^(file:\/{3})/,"")
                folderPath = decodeURIComponent(folderPath)
                receiveFileNameField.text = folderPath

                receiver.setFilePath(folderPath)
            }
        }

        Popup {
            id: fileExistsPopup
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 300
            height: 200
            modal: true
            closePolicy: Popup.NoAutoClose
            ColumnLayout {
                anchors.fill: parent
                Label {
                    text: qsTr("File already exists. Enter a new name: ")
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                }
                TextField {
                    id: fileExistsField
                    Layout.fillWidth: true
                    selectByMouse: true
                }
                Row {
                    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                    spacing: 10
                    Button {
                        text: qsTr("Create")
                        width: 75
                        onClicked: {
                            fileExistsPopup.close()
                            receiver.createFile(fileExistsField.text)
                        }
                    }
                    Button {
                        text: qsTr("Cancel")
                        width: 75
                        onClicked: {
                            fileExistsPopup.close()
                            receiver.sendFileError()
                        }
                    }
                }
            }
        }

        Popup {
            id: fileErrorPopup
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 300
            height: 100
            modal: true
            closePolicy: Popup.NoAutoClose
            ColumnLayout {
                anchors.fill: parent
                Label {
                    text: qsTr("There was an error creating the file.")
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                }
                Button {
                    text: qsTr("Ok")
                    Layout.preferredWidth: 75
                    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                    onClicked: {
                        fileErrorPopup.close()
                    }
                }
            }
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

                    TextField {
                        id: receiveFileNameField
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
                            receiveFileNameField.text = documents
                        }
                    }

                    Button {
                        id: receiveFileBrowseButton
                        width: 75
                        text: qsTr("Browse")
                        onClicked: receiveFileDialog.open()
                    }
                    spacing: 10
                }

                Row {
                    id: receiveFileProgressRow
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    spacing: 25
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
                }

                Row {
                    id: receiveFileUtilitiesRow
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 10
                    Button {
                        id: receiveFilePauseButton
                        width: 100
                        text: qsTr("Pause")
                        onClicked: {
                            if (this.text === "Pause")
                                receiver.pauseFileTransfer()
                            else
                                receiver.resumeFileTransfer()
                        }
                    }
                    Button {
                        id: receiveFileCancelButton
                        width: 100
                        text: qsTr("Cancel")
                        onClicked: receiver.cancelFileTransfer()
                    }
                }
            }

        }

    }
}
