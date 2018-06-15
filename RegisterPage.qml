import QtQuick 2.10
import QtQuick.Controls 2.3
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
            text: qsTr("Add Devices")
            font.bold: true
            font.family: "Segoe UI"
            Layout.columnSpan: 1
            Layout.rowSpan: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.pointSize: 30
        }

        Label {
            id: thisKeyLabel
            font.family: "Segoe UI"
            Layout.columnSpan: 1
            Layout.rowSpan: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.pointSize: 25
        }

        Label {
            id: invalidKey
            color: "#e00000"
            text: qsTr("*Invalid Device Key")
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
                    placeholderText: qsTr("Add 8-digit Device Key")
                    text: deviceKey
                    echoMode: TextInput.Normal
                    Layout.preferredWidth: 360
                    Layout.preferredHeight: 40
                    Layout.fillWidth: true
                    maximumLength: 8
                    selectByMouse: true
                    readOnly: readOnlyStatus
                    onEditingFinished: deviceKey = text
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
                        else if (registerDeviceList.checkNewDeviceKey())
                        {
                            registerDeviceList.insertItem()
                            textField.text = ""
                            invalidKey.visible = false
                        }
                        else
                            invalidKey.visible = true
                    }
                }

            }
            model: RegisterDeviceModel {
                deviceList: registerDeviceList
                Component.onCompleted: {
                    thisKeyLabel.text = qsTr("Your device key: " +
                                            registerDeviceList.getThisKey())
                }
            }
        }
    }

}
