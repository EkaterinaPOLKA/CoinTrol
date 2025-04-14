import QtQuick
import QtQuick.Controls.Basic
import "../Source"

Button {
    id: control
    width: 40
    height: 40
    background: Rectangle {
        color: (control.hovered || control.pressed) ? color_kard_kategory : color_transparent_button
        border.color: color_black
        anchors.fill: parent
        radius: 10
        CustomText24 {
            text: "+"
            color: color_background_main
            anchors.centerIn: parent
        }
    }
}
