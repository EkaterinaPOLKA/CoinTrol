import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick

import "./../Source"

Dialog {
    id: deleteTranz
    width: 600
    height: 150
    anchors.centerIn: parent
    background: Rectangle {
        color: color_kard_kategory
        border.color: color_black
        radius: 10
    }
    visible: deleteWdialog
    modal: true
    focus: true
    z: 10
    opacity: 0.9

    onAccepted: {
        deleteWdialog = false;
    }
    onRejected: {
        deleteWdialog = false;
    }

    property string nameWallet: ""

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
        z: 15
        onClicked: resetInactivityTimer()
        onPressed: resetInactivityTimer()
        onReleased: resetInactivityTimer()
        onPositionChanged: resetInactivityTimer()
    }

    CustomText34 {
        id: textTranzEdit
        color: color_background_main
        text: "Удаление кошелька"
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 40
        wrapMode: Text.WordWrap
        font.bold: true
    }

    Row {
        id: rowBtn
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        spacing: 20
        CustomButton {
            text: "Удалить"
            onClicked: {
                if (transactionModel.removeWallet(dbManager.userId, nameWallet)) {
                    deleteWdialog = false;
                } else {
                    deleteWarning.text = "Ошибка удаления кошелька";
                    deleteWarning.visible = true;
                }
            }
        }
        CustomButton {
            text: "Отмена"
            onClicked: {
                deleteWarning.visible = false
                deleteWdialog = false
            }
        }
    }

    CustomText14 {
        id: deleteWarning
        visible: false
        color: "red"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
