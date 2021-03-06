import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Page {
    id: connectionUnrecognizedPage

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Label {
            id: connectionUnrecognizedLabel
            text: qsTr('Device keys do not match.\n' +
                       'Make sure both are registered.')
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.bold: true
            font.pointSize: 20
        }

        Button {
            id: connectionUnrecognizedReturnButton
            text: qsTr('Return')
            focusPolicy: Qt.NoFocus
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.preferredHeight: 40
            Layout.preferredWidth: 150
            onClicked: window.pop(null)
        }
    }
}
