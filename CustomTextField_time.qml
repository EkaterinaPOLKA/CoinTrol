import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

TextField {
    id: timeField
    font.pixelSize: 16
    font.bold: false
    color: color_black
    font.family: "Arial"
    validator: RegularExpressionValidator {
        regularExpression: /^(0[0-9]|1[0-9]|2[0-3]|[0-9]):[0-5][0-9]$/
    }
    property string previousText: ""
    onTextChanged: {
        let timePattern = /^(0[0-9]|1[0-9]|2[0-3]|[0-9]):[0-5][0-9]$/;
        if (!timePattern.test(text)) {
            timeField.color = "red"; // Установить цвет, если формат неверный
        } else {
            timeField.color = color_black; // Восстановить цвет
        }
        if (previousText !== text) {
            previousText = text;
            forceActiveFocus();
        }
    }
}
