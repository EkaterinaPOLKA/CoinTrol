import QtQuick
import QtQuick.Controls.Basic

Switch {
    id: control
    indicator: Rectangle {
        width: 54
        height: 24
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 10
        color: control.checked ? color_background_header : color_transparent_button
        border.color: color_black
        border.width: 2

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            x: control.checked ? parent.width - width - 4 : 4
            width: 22
            height: 18
            radius: 6
            color: color_black
        }
    }
}
