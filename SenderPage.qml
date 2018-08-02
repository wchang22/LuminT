import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.0
import Qt.labs.folderlistmodel 2.2

import communications 1.0

ScrollView {
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Page {
        id: senderPage
        padding: 15
        width: (Qt.platform.os == 'android') ? Screen.width : 480

        Component.onCompleted:
        {
            if (sender.getMessageState() !== Sender.MessageState.FILE_PAUSED)
                return;

            sendTextArea.enabled = false;
            sendTextButtonRow.enabled = false;
            sendFileRow.enabled = false;
            sendFileButtonRow.enabled = false;

            sendFileNameField.text = sender.getCurrentPath();
            sendFileProgressBar.value = sender.getCurrentProgress();
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

        FileDialog {
            id: senderFileDialog
            title: qsTr('Select a file')
            folder: (Qt.platform.os == 'android') ?
                StandardPaths.standardLocations(StandardPaths.GenericDataLocation)[0] :
                StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
            onAccepted:
            {
                sendFileNameField.text = senderPage.cleanPath(senderFileDialog.file.toString());
            }
        }

        Popup {
            id: senderFileDialogAndroid
            width: parent.width
            height: Screen.height * 0.75
            closePolicy: Popup.NoAutoClose
            property string selectedFile
            selectedFile: ''

            ListView {
                anchors.fill: parent
                boundsMovement: Flickable.StopAtBounds
                displayMarginBeginning: -40
                displayMarginEnd: -88

                FolderListModel {
                    id: senderFileDialogAndroidModel
                    showDirsFirst: true
                    folder: (Qt.platform.os == 'android') ?
                        StandardPaths.standardLocations(StandardPaths.GenericDataLocation)[0] + '/'
                        : StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0] + '/'
                }

                Component {
                    id: senderFileDialogAndroidDelegate
                    RowLayout {
                        width: parent.width
                        spacing: 0

                        TextField {
                            id: senderFileDialogAndroidItem
                            text: fileName
                            font.pointSize: 12
                            padding: 15
                            Layout.preferredWidth: fileIsDir ? parent.width : parent.width * 0.8
                            readOnly: true

                            onReleased:
                            {
                                if (!fileIsDir)
                                {
                                    senderFileDialogAndroid.selectedFile = text;
                                }
                                else
                                {
                                    senderFileDialogAndroidModel.folder += fileName + '/';
                                    senderFileDialogAndroid.selectedFile = '';
                                }
                            }

                            onActiveFocusChanged:
                            {
                                // TODO: bug if tab is clicked - won't go back to gray
                                if (senderFileDialogAndroid.selectedFile == text)
                                    this.background.border.color = 'blue';
                                else
                                    this.background.border.color = 'gray';
                            }

                            onHoveredChanged:
                            {
                                if (this.hovered)
                                    this.background.color = 'white';
                                else
                                    this.background.color = 'transparent';
                            }

                            onVisibleChanged:
                            {
                                if (!this.visible)
                                    this.background.border.color = 'gray';
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
                                // TODO: fails if item is created after list is loaded
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
                    id: senderFileDialogAndroidHeader
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
                                        senderFileDialogAndroidModel.folder ==
                                        StandardPaths.standardLocations(
                                            StandardPaths.GenericDataLocation)[0] + '/')
                                        return;

                                    var pathList = senderPage.cleanPath(
                                                   senderFileDialogAndroidModel.folder).split('/');

                                    if (pathList.length <= 2)
                                        return;

                                    var newPathList = pathList.slice(
                                        ((Qt.platform.os == 'windows') ? 0 : 1), -2);
                                    senderFileDialogAndroidModel.folder =
                                        'file:///' + newPathList.join('/') + '/';
                                    senderFileDialogAndroid.selectedFile = '';
                                }
                            }

                            TextField {
                                width: parent.width * 0.8
                                text: senderPage.cleanPath(senderFileDialogAndroidModel.folder)
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
                    id: senderFileDialogAndroidFooter
                    DialogButtonBox {
                        z: 2
                        standardButtons: Dialog.Ok | Dialog.Cancel
                        width: parent.width
                        onAccepted:
                        {
                            if (senderFileDialogAndroid.selectedFile.length === 0)
                            {
                                senderFileDialogAndroid.open();
                                return;
                            }

                            senderFileDialogAndroid.close();
                            sendFileNameField.text = senderPage.cleanPath(
                                                        senderFileDialogAndroidModel.folder) +
                                                        senderFileDialogAndroid.selectedFile;
                            senderFileDialogAndroid.selectedFile = '';
                        }
                        onRejected:
                        {
                            senderFileDialogAndroid.selectedFile = '';
                            senderFileDialogAndroid.close();
                        }
                    }
                }

                model: senderFileDialogAndroidModel
                delegate: senderFileDialogAndroidDelegate
                header: senderFileDialogAndroidHeader
                headerPositioning: ListView.OverlayHeader
                footer: senderFileDialogAndroidFooter
                footerPositioning: ListView.OverlayFooter
            }
        }

        Connections {
            target: sender
            onSendProgress: sendFileProgressBar.value = progress
            onFileError:
            {
                sendTextArea.enabled = true;
                sendTextButtonRow.enabled = true;
                sendFileRow.enabled = true;
                sendFileButtonRow.enabled = true;
            }
            onFileCompleted:
            {
                sendTextArea.enabled = true;
                sendTextButtonRow.enabled = true;
                sendFileRow.enabled = true;
                sendFileButtonRow.enabled = true;
            }
        }

        ColumnLayout {
            id: senderColumnLayout
            anchors.fill: parent
            spacing: 30

            ColumnLayout {
                id: sendTextColumnLayout
                Layout.fillWidth: true
                spacing: 20

                Label {
                    id: sendTextLabel
                    text: qsTr('Send Text')
                    font.family: 'Segoe UI'
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    font.bold: true
                    font.pointSize: 20
                }

                TextArea {
                    id: sendTextArea
                    text: qsTr('')
                    Layout.fillWidth: true
                    Layout.minimumHeight: 100
                    horizontalAlignment: Text.AlignLeft
                    placeholderText: 'Enter Text'
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    selectByMouse: true
                }

                Row {
                    id: sendTextButtonRow
                    height: 50
                    Layout.fillWidth: true
                    spacing: 5

                    Button {
                        id: clipboardCopyButton
                        width: (parent.width - parent.spacing * 3) / 4
                        text: qsTr('Copy')
                        onClicked: utilities.copy(sendTextArea.text)
                    }
                    Button {
                        id: clipboardPasteButton
                        width: (parent.width - parent.spacing * 3) / 4
                        text: qsTr('Paste')
                        onClicked: sendTextArea.text = utilities.paste()
                    }
                    Button {
                        id: clearButton
                        width: (parent.width - parent.spacing * 3) / 4
                        text: qsTr('Clear')
                        onClicked: sendTextArea.text = ''
                    }

                    Button {
                        id: sendButton
                        width: (parent.width - parent.spacing * 3) / 4
                        text: qsTr('Send')
                        onClicked: sender.sendTextMessage(sendTextArea.text)
                    }
                }
            }

            ColumnLayout {
                id: sendFileColumnLayout
                Layout.fillWidth: true
                spacing: 20

                Label {
                    id: sendFileLabel
                    text: qsTr('Send File')
                    Layout.alignment: Qt.AlignHCenter
                    font.bold: true
                    font.pointSize: 20
                }

                Row {
                    id: sendFileRow
                    Layout.fillWidth: true
                    spacing: 5

                    Label {
                        id: sendFileNameLabel
                        text: qsTr('File:')
                        verticalAlignment: Text.AlignVCenter
                        width: (parent.width - parent.spacing) * 0.25
                        height: 32
                    }

                    TextField {
                        id: sendFileNameField
                        width: (parent.width - parent.spacing) * 0.75
                        text: qsTr('')
                        readOnly: true
                        selectByMouse: true
                    } 
                }

                Row {
                    id: sendFileButtonRow
                    layoutDirection: Qt.RightToLeft
                    Layout.fillWidth: true
                    spacing: 5

                    Button {
                        id: sendFileButton
                        width: (parent.width - parent.spacing * 3) * 0.25
                        text: qsTr('Send')
                        onClicked: {
                            if (sendFileNameField.text.length != 0)
                            {
                                sender.sendFile(sendFileNameField.text);
                                sendTextArea.enabled = false;
                                sendTextButtonRow.enabled = false;
                                sendFileRow.enabled = false;
                                sendFileButtonRow.enabled = false;
                            }
                        }
                    }

                    Button {
                        id: sendFileBrowseButton
                        width: (parent.width - parent.spacing * 3) * 0.25
                        text: qsTr('Browse')
                        onClicked:
                        {
                            if (Qt.platform.os == 'android')
                                senderFileDialogAndroid.open();
                            else
                                senderFileDialog.open();
                        }
                    }
                }

                Row {
                    id: sendFileProgressRow
                    Layout.fillWidth: true
                    spacing: 5

                    Label {
                        id: sendFileProgressLabel
                        text: qsTr('Progress:')
                        width: (parent.width - parent.spacing) * 0.25
                    }

                    ProgressBar {
                        id: sendFileProgressBar
                        width: (parent.width - parent.spacing) * 0.75
                        height: 20
                        value: 0
                    }

                }
            }
        }
    }
}
