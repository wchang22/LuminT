import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0
import QtQuick.Dialogs 1.2

import utilities 1.0
import communications 1.0

ScrollView {
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Page {
        id: receiverPage
        padding: 15
        width: (Qt.platform.os == "android") ? Screen.width : 480

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
            y: (parent.height - height) / 2
            width: parent.width
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
            y: (parent.height - height) / 2
            width: parent.width
            height: 200
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
            spacing: 30

            ColumnLayout {
                id: receiveTextColumnLayout
                Layout.fillWidth: true
                spacing: 20

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
                    Layout.fillWidth: true
                    Layout.minimumHeight: 100
                    horizontalAlignment: Text.AlignLeft
                    selectByMouse: true
                    readOnly: true
                }

                Row {
                    id: receiveTextButtonRow
                    Layout.fillWidth: true
                    height: 50
                    spacing: 5

                    Button {
                        id: clipboardCopyButton
                        width: (parent.width - parent.spacing * 2) / 3
                        text: qsTr("Copy")
                        onClicked: utilities.copy(receiveTextArea.text)
                    }

                    Button {
                        id: clipboardPasteButton
                        width: (parent.width - parent.spacing * 2) / 3
                        text: qsTr("Paste")
                        onClicked: receiveTextArea.text = utilities.paste()
                    }

                    Button {
                        id: clearButton
                        width: (parent.width - parent.spacing * 2) / 3
                        text: qsTr("Clear")
                        onClicked: receiveTextArea.text = ""
                    }
                }
            }

            ColumnLayout {
                id: receiveFileColumnLayout
                Layout.fillWidth: true
                spacing: 20

                Label {
                    id: receiveFileLabel
                    text: qsTr("Receive File")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    font.bold: true
                    font.pointSize: 20
                }

                Row {
                    id: receiveFileRow
                    Layout.fillWidth: true
                    spacing: 5

                    Label {
                        id: receiveFileNameLabel
                        text: qsTr("Destination:")
                        verticalAlignment: Text.AlignVCenter
                        width: (parent.width - parent.spacing) * 0.25
                        height: 32
                    }

                    TextField {
                        id: receiveFileNameField
                        width: (parent.width - parent.spacing) * 0.75
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
                }

                Row {
                    id: receiveFileButtonRow
                    Layout.fillWidth: true
                    layoutDirection: Qt.RightToLeft

                    Button {
                        id: receiveFileBrowseButton
                        width: (parent.width - parent.spacing) * 0.25
                        text: qsTr("Browse")
                        onClicked: receiveFileDialog.open()
                    }
                }

                Row {
                    id: receiveFileProgressRow
                    Layout.fillWidth: true
                    spacing: 5

                    Label {
                        id: receiveFileProgressLabel
                        text: qsTr("Progress:")
                        width: (parent.width - parent.spacing) * 0.25
                    }

                    ProgressBar {
                        id: receiveFileProgressBar
                        width: (parent.width - parent.spacing) * 0.75
                        height: 20
                        value: 0
                    }
                }

                Row {
                    id: receiveFileUtilitiesRow
                    Layout.fillWidth: true
                    layoutDirection: Qt.RightToLeft
                    spacing: 5

                    Button {
                        id: receiveFileCancelButton
                        width: (parent.width - parent.spacing) * 0.25
                        text: qsTr("Cancel")
                        onClicked: receiver.cancelFileTransfer()
                    }

                    Button {
                        id: receiveFilePauseButton
                        width: (parent.width - parent.spacing) * 0.25
                        text: qsTr("Pause")
                        onClicked: {
                            if (this.text === "Pause")
                                receiver.pauseFileTransfer()
                            else
                                receiver.resumeFileTransfer()
                        }
                    }

                }
            }

        }

    }
}
