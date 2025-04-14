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
        Image {
            width: 13
            height: 24
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 12
            source: "../Source/Str_left.svg"
        }
    }
}
