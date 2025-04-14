import QtQuick
import QtQuick.Controls.Basic

SpinBox {
    id: control
    width: 296
    height: 54

    background: Rectangle {
        color: color_white
        border.color: color_dark
        width: control.width
        height: control.height
    }

    up.indicator: {}
    down.indicator: {}

    contentItem: Rectangle {
        color: color_white
        border.color: color_black
        anchors.centerIn: parent
        anchors.fill: parent

        CustomTextField {
            id: inputField
            width: control.width - 40
            text: control.value.toString()
            validator: IntValidator { bottom: control.from; top: control.to }
            background: {}
            verticalAlignment: TextInput.AlignVCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.fill: parent
            placeholderText: "24"

            onTextChanged: {
                var newValue = parseInt(inputField.text);
                if (!isNaN(newValue) && newValue >= control.from && newValue <= control.to) {
                    control.value = newValue;
                }
            }

            Keys.onPressed: function(event) {
                if (event.key === Qt.Key_Up && control.value < control.to) {
                    upButton.color = color_dark;
                    control.value += control.stepSize;
                    upButtonResetTimer.restart();
                } else if (event.key === Qt.Key_Down && control.value > control.from) {
                    downButton.color = color_dark;
                    control.value -= control.stepSize;
                    downButtonResetTimer.restart();
                }
            }
        }

        Column {
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            spacing: -2

            CustomText24 {
                id: upButton
                text: "▲"
                color: control.value < control.to ? color_button : color_background_menu

                MouseArea {
                    id: upMouseArea
                    anchors.fill: parent
                    onPressed: {
                        upButton.color = color_dark;
                    }
                    onReleased: {
                        upButton.color = control.value < control.to ? color_button : color_background_menu;
                        if (control.value < control.to) {
                            control.value += control.stepSize;
                        }
                    }
                }
            }

            CustomText24 {
                id: downButton
                text: "▼"
                color: control.value > control.from ? color_button : color_background_menu

                MouseArea {
                    id: downMouseArea
                    anchors.fill: parent
                    onPressed: {
                        downButton.color = color_dark;
                    }
                    onReleased: {
                        downButton.color = control.value > control.from ? color_button : color_background_menu;
                        if (control.value > control.from) {
                            control.value -= control.stepSize;
                        }
                    }
                }
            }
        }
    }

    Timer {
        id: upButtonResetTimer
        interval: 100
        repeat: false
        onTriggered: {
            upButton.color = control.value < control.to ? color_button : color_background_menu;
        }
    }

    Timer {
        id: downButtonResetTimer
        interval: 100
        repeat: false
        onTriggered: {
            downButton.color = control.value > control.from ? color_button : color_background_menu;
        }
    }

    onValueChanged: {
        inputField.text = control.value.toString();
    }
}
