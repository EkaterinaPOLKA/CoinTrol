import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

ComboBox {
    id: control
    width: 296
    height: 54
    background: Rectangle {
        anchors.fill: parent
        color: color_background_menu
        border.color: color_black
    }
    contentItem: CustomText24 {
        text: control.displayText
        anchors.fill: parent
        color: color_black
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        leftPadding: 10
        rightPadding: 30
        elide: Text.ElideRight
    }
    indicator: Rectangle {
        width: 30
        height: 30
        color: color_background_menu
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        CustomText24 {
            text: "▼"
            topPadding: 5
            color: color_black
        }
    }
    delegate: ItemDelegate {
        width: control.width
        contentItem: Rectangle {
            anchors.fill: parent
            color: control.currentIndex === index || hovered ? color_kard_kategory : color_background_menu
            height: 36
            CustomText24 {
                anchors.left: parent.left
                text: modelData
                color: color_black
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 10
                anchors.rightMargin: 30
                anchors.fill: parent
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
