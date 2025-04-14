import QtQuick
import QtQuick.Controls.Basic
import "../Source"

Button {
    id: control
    contentItem: CustomText24 {
        id: contentItem
        font.pixelSize: 16
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
        color: color_background_main
        text: control.text
    }
    height: 40
    implicitWidth: Math.max(contentItem.implicitWidth + 20, contentItem.width + 20)
    background: Rectangle {
        color: (control.hovered || control.pressed) ? color_kard_kategory : color_transparent_button
        border.color: color_black
        anchors.fill: parent
        radius: 10
    }
}
