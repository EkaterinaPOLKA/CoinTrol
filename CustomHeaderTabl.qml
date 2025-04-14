import QtQuick
import QtQuick.Controls.Basic

Button {
    id: control
    height: 30
    background: Rectangle {
        color: color_kard_kategory
        border.color: color_black
    }
    contentItem: CustomText14 {
        font.pixelSize: 16
        font.bold: true
        color: color_background_main
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
        text: control.text
        font.underline: control.checked
    }
}
