import QtQuick
import QtQuick.Controls.Basic

TabButton {
    id: control
    contentItem: CustomText24 {
        text: control.text
        color: control.checked ? color_background_main : color_black
        font.bold: control.checked ? true : false
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
    }
    background: Rectangle {
        color: control.checked ? color_kard_kategory : color_background_menu
        border.color: color_black
        anchors.fill: parent
    }
}
