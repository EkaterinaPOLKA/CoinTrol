import QtQuick
import QtQuick.Controls.Basic

Button {
    id: control
    enabled: false
    height: parent.height
    contentItem: CustomText14 {
        font.pixelSize: 12
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
        text: control.text
        wrapMode: Text.WordWrap
    }
    background: Rectangle {
        color: color_white
        border.color: color_black
    }
}
