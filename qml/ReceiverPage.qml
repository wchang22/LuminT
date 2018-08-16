import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0
import QtQuick.Dialogs 1.2
import Qt.labs.folderlistmodel 2.2

import utilities 1.0
import communications 1.0

ScrollView {
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Page {
        id: receiverPage
        padding: 15
        width: (Qt.platform.os == 'android') ? Screen.width : 480

        Component.onCompleted:
        {
            if (receiver.getMessageState() !== Receiver.MessageState.FILE_PAUSED)
                return;

            receiverFilePauseButton.text = 'Resume'
            receiverFolderNameField.text = receiver.getCurrentPath();
            receiverFileProgressBar.value = receiver.getCurrentProgress();
        }

        function cleanPath(path)
        {
            var filePath;
            if (Qt.platform.os == 'windows')
                filePath = path.toString().replace(/^(file:\/{3})/,'');
            else
                filePath = path.toString().replace(/^(file:\/{2})/,'');
            filePath = decodeURIComponent(filePath);
            return filePath;
        }

        Connections {
            target: receiver
            onReceivedText: receiverTextArea.text = text
            onReceiveProgress:
            {
                receiverFileProgressBar.value = progress;
                if (receiverFilePauseButton.text == 'Resume')
                    receiverFilePauseButton.text = 'Pause';
            }
            onFileStatus: {
                switch(status)
                {
                    case Receiver.FileState.EXISTS:
                        fileExistsField.text = fileName;
                        fileExistsPopup.open();
                        break
                    case Receiver.FileState.ERROR:
                        fileErrorPopup.open();
                        break
                    case Receiver.FileState.OK:
                        receiver.requestFirstPacket();
                }
            }
            onFilePaused: receiverFilePauseButton.text = 'Resume'
            onFileResumed: receiverFilePauseButton.text = 'Pause'
        }

        FolderDialog {
            id: receiverFolderDialog
            title: qsTr('Select a folder')
            folder: StandardPaths.standardLocations(
                        StandardPaths.DocumentsLocation)[0]
            onAccepted: {
                receiverFolderNameField.text = receiverPage.cleanPath(
                    receiverFolderDialog.folder.toString());

                receiver.setFilePath(receiverFolderNameField.text);
            }
        }

        Popup {
            id: receiverFolderDialogAndroid
            width: parent.width
            height: Screen.height * 0.75
            closePolicy: Popup.NoAutoClose

            ListView {
                anchors.fill: parent
                boundsMovement: Flickable.StopAtBounds
                displayMarginBeginning: -40
                displayMarginEnd: -88

                FolderListModel {
                    id: receiverFolderDialogAndroidModel
                    showDirsFirst: true
                    folder: (Qt.platform.os == 'android') ?
                        StandardPaths.standardLocations(StandardPaths.GenericDataLocation)[0] + '/'
                        : StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0] + '/'
                }

                Component {
                    id: receiverFolderDialogAndroidDelegate
                    RowLayout {
                        width: parent.width
                        spacing: 0

                        TextField {
                            id: receiverFolderDialogAndroidItem
                            text: fileName
                            font.pointSize: 12
                            padding: 15
                            Layout.preferredWidth: fileIsDir ? parent.width : parent.width * 0.8
                            readOnly: true

                            onReleased:
                            {
                                if (!fileIsDir)
                                    return;

                                receiverFolderDialogAndroidModel.folder += fileName + '/';
                            }

                            onHoveredChanged:
                            {
                                if (this.hovered)
                                    this.background.color = 'white';
                                else
                                    this.background.color = 'transparent';
                            }

                            Component.onCompleted:
                            {
                                this.background.color = 'transparent';
                                this.background.border.width = 1;
                                this.background.border.color = 'gray';
                            }
                        }

                        Label {
                            font.pointSize: 12
                            padding: 5
                            visible: fileIsDir ? false : true
                            Layout.preferredWidth: parent.width * 0.2
                            Component.onCompleted:
                            {
                                var magnitude = Math.floor(Math.log(fileSize) / Math.log(1000));
                                var magnitudeToByteUnit = ['B', 'KB', 'MB', 'GB'];
                                this.text = (fileSize / Math.pow(1024, magnitude))
                                             .toFixed(2).toString() + ' ' +
                                             magnitudeToByteUnit[magnitude];
                            }
                        }
                    }
                }

                Component {
                    id: receiverFolderDialogAndroidHeader
                    Rectangle {
                        width: parent.width
                        height: 32
                        z: 2

                        Row {
                            width: parent.width

                            Button {
                                text: qsTr('‹')
                                width: parent.width * 0.2
                                onClicked:
                                {
                                    if (Qt.platform.os == 'android' &&
                                        receiverFolderDialogAndroidModel.folder ==
                                        StandardPaths.standardLocations(
                                            StandardPaths.GenericDataLocation)[0] + '/')
                                        return;

                                    var pathList = receiverPage.cleanPath(
                                        receiverFolderDialogAndroidModel.folder).split('/');

                                    if (pathList.length <= 2)
                                        return;

                                    var newPathList = pathList.slice(
                                        ((Qt.platform.os == 'windows') ? 0 : 1), -2);
                                    receiverFolderDialogAndroidModel.folder =
                                        'file:///' + newPathList.join('/') + '/';
                                }
                            }

                            TextField {
                                width: parent.width * 0.8
                                text: receiverPage.cleanPath(
                                          receiverFolderDialogAndroidModel.folder)
                                readOnly: true
                                Component.onCompleted:
                                {
                                    this.background.color = 'transparent';
                                    this.background.border.width = 1;
                                }
                            }
                        }
                    }
                }

                Component {
                    id: receiverFolderDialogAndroidFooter
                    DialogButtonBox {
                        z: 2
                        standardButtons: Dialog.Ok | Dialog.Cancel
                        width: parent.width
                        onAccepted:
                        {
                            receiverFolderDialogAndroid.close();
                            receiverFolderNameField.text = receiverPage.cleanPath(
                                                        receiverFolderDialogAndroidModel.folder);
                            receiver.setFilePath(receiverFolderNameField.text);
                        }
                        onRejected:
                        {
                            receiverFolderDialogAndroid.close();
                        }
                    }
                }

                model: receiverFolderDialogAndroidModel
                delegate: receiverFolderDialogAndroidDelegate
                header: receiverFolderDialogAndroidHeader
                headerPositioning: ListView.OverlayHeader
                footer: receiverFolderDialogAndroidFooter
                footerPositioning: ListView.OverlayFooter
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
                    text: qsTr('File already exists. Enter a new name: ')
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
                        text: qsTr('Create')
                        width: 75
                        onClicked: {
                            fileExistsPopup.close();
                            receiver.createFile(fileExistsField.text);
                        }
                    }
                    Button {
                        text: qsTr('Cancel')
                        width: 75
                        onClicked: {
                            fileExistsPopup.close();
                            receiver.sendFileError();
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
                    text: qsTr('There was an error creating the file.')
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                }
                Button {
                    text: qsTr('Ok')
                    Layout.preferredWidth: 75
                    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                    onClicked: {
                        fileErrorPopup.close();
                        receiver.sendFileError();
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
                    id: receiverTextLabel
                    text: qsTr('Receive Text')
                    font.family: 'Segoe UI'
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    font.bold: true
                    font.pointSize: 20
                }

                TextArea {
                    id: receiverTextArea
                    text: qsTr('')
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    placeholderText: 'Wait for text'
                    Layout.fillWidth: true
                    Layout.minimumHeight: 100
                    horizontalAlignment: Text.AlignLeft
                    selectByMouse: true
                    readOnly: true
                }

                Row {
                    id: receiverTextButtonRow
                    Layout.fillWidth: true
                    height: 50
                    spacing: 5

                    Button {
                        id: clipboardCopyButton
                        width: (parent.width - parent.spacing * 2) / 3
                        text: qsTr('Copy')
                        onClicked: utilities.copy(receiverTextArea.text)
                    }

                    Button {
                        id: clipboardPasteButton
                        width: (parent.width - parent.spacing * 2) / 3
                        text: qsTr('Paste')
                        onClicked: receiverTextArea.text = utilities.paste()
                    }

                    Button {
                        id: clearButton
                        width: (parent.width - parent.spacing * 2) / 3
                        text: qsTr('Clear')
                        onClicked: receiverTextArea.text = ''
                    }
                }
            }

            ColumnLayout {
                id: receiverFileColumnLayout
                Layout.fillWidth: true
                spacing: 20

                Label {
                    id: receiverFileLabel
                    text: qsTr('Receive File')
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    font.bold: true
                    font.pointSize: 20
                }

                Row {
                    id: receiverFolderRow
                    Layout.fillWidth: true
                    spacing: 5

                    Label {
                        id: receiverFolderNameLabel
                        text: qsTr('Destination:')
                        verticalAlignment: Text.AlignVCenter
                        width: (parent.width - parent.spacing) * 0.25
                        height: 32
                    }

                    TextField {
                        id: receiverFolderNameField
                        width: (parent.width - parent.spacing) * 0.75
                        readOnly: true
                        selectByMouse: true
                        Component.onCompleted: {
                            var location = (Qt.platform.os == 'android') ?
                                            StandardPaths.standardLocations(
                                            StandardPaths.GenericDataLocation)[0] + '/' :
                                            StandardPaths.standardLocations(
                                            StandardPaths.DocumentsLocation)[0] + '/';
                            receiverFolderNameField.text = receiverPage.cleanPath(location);
                        }
                    } 
                }

                Row {
                    id: receiverFolderButtonRow
                    Layout.fillWidth: true
                    layoutDirection: Qt.RightToLeft

                    Button {
                        id: receiverFolderBrowseButton
                        width: (parent.width - parent.spacing) * 0.25
                        text: qsTr('Browse')
                        onClicked: {
                            if (Qt.platform.os == 'android')
                                receiverFolderDialogAndroid.open();
                            else
                                receiverFolderDialog.open();
                        }
                    }
                }

                Row {
                    id: receiverFileProgressRow
                    Layout.fillWidth: true
                    spacing: 5

                    Label {
                        id: receiverFileProgressLabel
                        text: qsTr('Progress:')
                        width: (parent.width - parent.spacing) * 0.25
                    }

                    ProgressBar {
                        id: receiverFileProgressBar
                        width: (parent.width - parent.spacing) * 0.75
                        height: 20
                        value: 0
                    }
                }

                Row {
                    id: receiverFileUtilitiesRow
                    Layout.fillWidth: true
                    layoutDirection: Qt.RightToLeft
                    spacing: 5

                    Button {
                        id: receiverFileCancelButton
                        width: (parent.width - parent.spacing) * 0.25
                        text: qsTr('Cancel')
                        onClicked: receiver.cancelFileTransfer()
                    }

                    Button {
                        id: receiverFilePauseButton
                        width: (parent.width - parent.spacing) * 0.25
                        text: qsTr('Pause')
                        onClicked: {
                            if (this.text === 'Pause')
                                receiver.pauseFileTransfer();
                            else
                                receiver.resumeFileTransfer();
                        }
                    }

                }
            }

        }

    }
}
