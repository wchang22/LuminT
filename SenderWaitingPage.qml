import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: senderWaitingPage
    objectName: "senderWaitingPage"

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.margins: 10

            Row {
                spacing: 10

                Label {
                    id: receiverIDLabel
                    text: qsTr("Enter Receiver ID: ")
                    font.pointSize: 20
                    font.bold: true
                }
                TextField {
                    id: receiverIDField
                    width: 100
                    font.pointSize: 15
                    placeholderText: qsTr("Receiver ID")
                    maximumLength: 3
                    selectByMouse: true
                    validator: RegExpValidator {
                        regExp: /[\d]+/
                    }
                }
                Button {
                    id: receiverIDButton
                    width: 80
                    text: qsTr("Start")
                    font.bold: true
                    focusPolicy: Qt.NoFocus
                    onClicked: {
                        sender.setPeerIPAddress(receiverIDField.text)
                        if (receiverIDField.text.length === 0 ||
                            parseInt(receiverIDField.text, 10) > 255 ||
                            parseInt(receiverIDField.text, 10) === 0)
                        {
                            invalidID.visible = true
                            receiverWaitingLabel.visible = false
                            receiverWaitingIndicator.visible = false
                        }
                        else
                        {
                            sender.connectToReceiver()
                            invalidID.visible = false
                            receiverWaitingLabel.visible = true
                            receiverWaitingIndicator.visible = true
                            receiverIDField.enabled = false
                            receiverIDButton.enabled = false
                        }
                    }
                }
            }
            Row {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Label {
                    id: invalidID
                    color: "#e00000"
                    text: qsTr("*Invalid ID")
                    font.family: "Segoe UI"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    font.pointSize: 15
                    visible: false;
                }
            }
        }

        Label {
            id: receiverWaitingLabel
            text: qsTr("Waiting for Receiver")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            font.bold: true
            font.pointSize: 40
            visible: false
        }

        BusyIndicator {
            id: receiverWaitingIndicator
            width: 150
            height: 150
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.preferredHeight: 150
            Layout.preferredWidth: 150
            visible: false
        }
    }
}
