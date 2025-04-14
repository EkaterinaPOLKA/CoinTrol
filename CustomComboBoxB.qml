import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

ComboBox {
    id: control
    height: 32
    background: Rectangle {
        anchors.fill: parent
        color: color_background_menu
        border.color: color_black
    }
    contentItem: CustomText14 {
        font.pixelSize: 16
        text: control.displayText
        anchors.fill: parent
        color: color_black
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        leftPadding: 10
        rightPadding: 30
    }
    indicator: Rectangle {
        width: 15
        height: 15
        color: color_background_menu
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        CustomText14 {
            text: "▼"
            font.pixelSize: 16
            color: color_black
        }
    }
    delegate: ItemDelegate {
        width: control.width
        contentItem: Rectangle {
            anchors.fill: parent
            color: control.currentIndex === index || hovered ? color_kard_kategory : color_background_menu
            height: 24
            CustomText14 {
                font.pixelSize: 16
                anchors.left: parent.left
                text: modelData
                color: color_black
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 10
            }
        }
    }
    popup: Popup {
        y: control.height + 1
        width: control.width
        implicitHeight: listview.contentHeight + 24
        focus: true
        contentItem: ListView {
            id: listview
            model: control.popup.visible ? control.delegateModel : null
            clip: true
            currentIndex: control.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }
        background: Rectangle {
            color: color_background_menu
            border.color: color_black
        }
    }
}
