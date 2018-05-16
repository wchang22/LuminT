import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import qml 1.0

Page {
    id: registerPage
    objectName: "registerPage"

    ColumnLayout {
        id: columnLayout
        spacing: 10
        anchors.fill: parent

        Label {
            id: label
            text: qsTr("Add Sender Devices")
            font.bold: true
            font.family: "Segoe UI"
            Layout.columnSpan: 1
            Layout.rowSpan: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.pointSize: 30
        }

        Label {
            id: thisIDLabel
            font.family: "Segoe UI"
            Layout.columnSpan: 1
            Layout.rowSpan: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.pointSize: 25
        }

        Label {
            id: invalidID
            color: "#e00000"
            text: qsTr("*Invalid Device ID")
            font.family: "Segoe UI"
            Layout.columnSpan: 1
            Layout.rowSpan: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.pointSize: 15
            visible: false;
        }

        ListView {
            id: listView
            boundsBehavior: Flickable.DragAndOvershootBounds
            ScrollBar.vertical: ScrollBar {
                width: 5
            }
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            spacing: 5
            clip: true

            delegate: RowLayout {
                width: 350
                layoutDirection: Qt.LeftToRight
                anchors.horizontalCenter: parent.horizontalCenter

                spacing: 5
                TextField {
                    id: textField
                    font.pointSize: 15
                    placeholderText: qsTr("Add 8-digit Device ID")
                    text: deviceID
                    echoMode: TextInput.Normal
                    Layout.preferredWidth: 360
                    Layout.preferredHeight: 40
                    Layout.fillWidth: true
                    maximumLength: 8
                    selectByMouse: true
                    readOnly: readOnlyStatus
                    onEditingFinished: deviceID = text
                    validator: RegExpValidator {
                        regExp: /[\da-z]*/
                    }
                }

                Button {
                    property int sequence: seq
                    id: registerButton
                    width: 50
                    height: 40
                    text: buttonText
                    rightPadding: 5
                    leftPadding: 5
                    padding: 5
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 50
                    font.pointSize: 20
                    font.bold: false
                    onClicked: {
                        if (this.sequence !== 0)
                            registerDeviceList.removeItem(this.sequence)
                        else if (registerDeviceList.checkNewDeviceID())
                        {
                            registerDeviceList.insertItem()
                            textField.text = ""
                            invalidID.visible = false
                        }
                        else
                            invalidID.visible = true
                    }
                }

            }
            model: RegisterDeviceModel {
                deviceList: registerDeviceList
                Component.onCompleted: {
                    thisIDLabel.text = qsTr("Your device ID: " +
                                            registerDeviceList.getThisID())
                }
            }
        }
    }

}
