import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("LuminT")

    StackView {
        id: window
        transformOrigin: Item.TopLeft
        anchors.fill: parent

        Page {
            transformOrigin: Item.Center
            anchors.fill: parent
            title: "LuminT"

            Label {
                id: title
                x: 242
                text: "LuminT"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 50
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 360
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                font.italic: true
                font.bold: true
                font.pointSize: 50
            }

            ColumnLayout {
                id: column
                width: 200
                spacing: 0
                anchors.top: parent.top
                anchors.topMargin: 200
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: send
                    width: 150
                    height: 50
                    text: qsTr("Send")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 150
                    anchors.verticalCenterOffset: -40
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: window.push(sender)
                }

                Button {
                    id: receive
                    width: 150
                    height: 50
                    text: qsTr("Receive")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 150
                    anchors.verticalCenterOffset: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: window.push(receiver)
                }

            }
        }

        Component {
            id: sender
            Page {
                anchors.fill: parent

                Label {
                    id: label
                    x: 154
                    y: 79
                    text: qsTr("Label")
                }
            }
        }

        Component {
            id: receiver
            Page {
                anchors.fill: parent

            }
        }

    }
}
