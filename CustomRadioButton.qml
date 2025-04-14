import QtQuick
import QtQuick.Controls.Basic

RadioButton {
    id: control
    indicator: Rectangle {
        width: 24
        height: 24
        radius: 12
        color: color_background_header
        border.color: color_black
        border.width: 2
        anchors.verticalCenter: parent.verticalCenter
        Rectangle {
            width: 10
            height: 10
            radius: 5
            color: color_black
            visible: control.checked
            anchors.centerIn: parent
        }
    }
    contentItem: CustomText24 {
        text: control.text
        anchors.verticalCenter: parent.verticalCenter
        leftPadding: 27
    }
}
