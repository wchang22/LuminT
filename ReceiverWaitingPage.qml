import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    id: receiverWaitingPage
    objectName: "receiverWaitingPage"

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.margins: 10

            Row {
                spacing: 10

                Label {
                    id: senderIDLabel
                    text: qsTr("Enter Sender ID: ")
                    font.pointSize: 20
                    font.bold: true
                }
                TextField {
                    id: senderIDField
                    width: 100
                    font.pointSize: 15
                    placeholderText: qsTr("Sender ID")
                    maximumLength: 3
                    selectByMouse: true
                    validator: RegExpValidator {
                        regExp: /[\d]+/
                    }
                }
                Button {
                    id: senderIDButton
                    width: 80
                    text: qsTr("Start")
                    font.bold: true
                    focusPolicy: Qt.NoFocus
                    onClicked: {
                        receiver.setPeerIPAddress(senderIDField.text)
                        if (senderIDField.text.length === 0 || !receiver.startServer())
                        {
                            invalidID.visible = true
                            senderWaitingLabel.visible = false
                            senderWaitingIndicator.visible = false
                        }
                        else
                        {
                            invalidID.visible = false
                            senderWaitingLabel.visible = true
                            senderWaitingIndicator.visible = true
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
            id: senderWaitingLabel
            text: qsTr("Waiting for Sender")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            font.bold: true
            font.pointSize: 40
            visible: false
        }

        BusyIndicator {
            id: senderWaitingIndicator
            width: 150
            height: 150
            Layout.preferredHeight: 150
            Layout.preferredWidth: 150
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: false
        }
    }
}
