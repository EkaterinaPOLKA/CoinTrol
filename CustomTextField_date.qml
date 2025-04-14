import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

TextField {
    id: dateField
    font.pixelSize: 16
    font.bold: false
    color: color_black
    font.family: "Arial"
    validator: RegularExpressionValidator {
        regularExpression: /^\d{2}\.\d{2}\.\d{4}$/
    }
    property string previousText: ""
    onTextChanged: {
        let datePattern = /^\d{2}\.\d{2}\.\d{4}$/;
        if (!datePattern.test(text)) {
            dateField.color = "red"; // Установить цвет, если формат неверный
        } else {
            dateField.color = color_black; // Восстановить цвет
        }
        if (previousText !== text) {
            previousText = text;
            forceActiveFocus();
        }
    }
}
