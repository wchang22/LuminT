import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Page {
    id: connectionErrorPage

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Label {
            id: connectionErrorLabel
            text: qsTr("Encryption failed.\nPlease try again.")
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.bold: true
            font.pointSize: 20
        }

        Button {
            id: connectionErrorReturnButton
            text: qsTr("Return")
            focusPolicy: Qt.NoFocus
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.preferredHeight: 40
            Layout.preferredWidth: 150
            onClicked: window.pop(null)
        }
    }
}
