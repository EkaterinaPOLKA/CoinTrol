import QtQuick.Controls.Basic
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick

Dialog {
    id: expenses
    width: 650
    height: parent.width / 2 - 100
    anchors.centerIn: parent
    background: Rectangle {
        color: color_kard_kategory
        border.color: color_black
        radius: 10
    }
    visible: isHint
    modal: true
    focus: true
    z: 10
    opacity: 0.9

    onAccepted: {
        isHint = false;
    }
    onRejected: {
        isHint = false;
    }

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
        id: textTranz
        color: color_background_block
        text: "Навигация по приложению"
        anchors.top: parent.top
        anchors.topMargin: 20
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 40
        wrapMode: Text.WordWrap
        font.bold: true
    }

    Button {
        id: control
        width: 30
        height: 30
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.verticalCenter: textTranz.verticalCenter
        background: Rectangle {
            anchors.fill: parent
            color: "transparent"
            Image {
                anchors.fill: parent
                source: "../Source/Close.svg"
            }
        }
        onClicked: isHint = false
    }

    Column {
        anchors.top: textTranz.bottom
        anchors.topMargin: 10
        width: parent.width
        spacing: 15

        Row {
            width: parent.width
            spacing: 15

            CustomText34 {
                text: "Главная:"
                color: color_background_block
                font.bold: true
                width: 140
                anchors.verticalCenter: parent.verticalCenter
            }
            CustomText24 {
                text: "Здесь отображается список ваших транзакций с возможностью добавления, изменения или удаления операций. Также представлен анализ финансовой активности."
                color: color_background_block
                wrapMode: Text.WordWrap
                width: parent.width - 150
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 2
            color: color_background_block
        }

        Row {
            width: parent.width
            spacing: 15

            CustomText34 {
                text: "График:"
                color: color_background_block
                font.bold: true
                width: 140
                anchors.verticalCenter: parent.verticalCenter
            }
            CustomText24 {
                text: "Этот раздел позволяет отслеживать динамику доходов и расходов с помощью графиков. Дополнительно доступны финансовые советы для улучшения бюджета."
                color: color_background_block
                wrapMode: Text.WordWrap
                width: parent.width - 150
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 2
            color: color_background_block
        }

        Row {
            width: parent.width
            spacing: 15

            CustomText34 {
                text: "Планы:"
                color: color_background_block
                font.bold: true
                width: 140
                anchors.verticalCenter: parent.verticalCenter
            }
            CustomText24 {
                text: "Создавайте заметки с финансовыми планами на неделю, месяц или год."
                color: color_background_block
                wrapMode: Text.WordWrap
                width: parent.width - 150
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 2
            color: color_background_block
        }

        Row {
            width: parent.width
            spacing: 15

            CustomText34 {
                text: "Цели:"
                color: color_background_block
                font.bold: true
                width: 140
                anchors.verticalCenter: parent.verticalCenter
            }
            CustomText24 {
                text: "Раздел для постановки и отслеживания финансовых целей. Добавляйте новые цели, следите за прогрессом и анализируйте достижения за весь период использования приложения."
                color: color_background_block
                wrapMode: Text.WordWrap
                width: parent.width - 150
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
