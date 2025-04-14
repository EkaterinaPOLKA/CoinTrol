import QtQuick
import QtQuick.Controls.Basic

Button {
    id: control
    width: 213
    height: 40
    contentItem: CustomText24 {
        text: control.text
        color: color_background_main
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
    }
    background: Rectangle {
        color: (control.hovered || control.pressed) ? color_black : color_button
        border.color: color_black
        anchors.fill: parent
    }
}
