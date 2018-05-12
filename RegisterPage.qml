import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import qml 1.0

Page {
    id: registerPage
    objectName: "registerPage"

    signal registerButtonClicked()

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
            font.pointSize: 50
        }

        ListView {
            id: listView
            boundsBehavior: Flickable.DragAndOvershootBounds
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            spacing: 5
            clip: true
            delegate: RowLayout {
                width: 470
                layoutDirection: Qt.LeftToRight
                anchors.horizontalCenter: parent.horizontalCenter

                spacing: 5
                TextField {
                    font.pointSize: 15
                    placeholderText: "Add Device ID"
                    text: deviceID
                    Layout.preferredWidth: 360
                    Layout.preferredHeight: 40
                    Layout.fillWidth: true
                    readOnly: readOnlyStatus
                }

                Button {
                    id: registerButton
                    objectName: "registerButton"
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
                    onClicked: registerButtonClicked()
                }

            }
            model: RegisterDeviceModel {

            }

//            model: ListModel {
//                id: listModel
//                ListElement {
//                    deviceID: ""
//                    readOnlyStatus: false
//                    buttonText: "+"
//                }
//                ListElement {
//                    textFieldID: "1"
//                    deviceID: "123456"
//                    readOnlyStatus: true
//                    buttonText: "\u2013"
//                }
//            }
        }

    }

}
