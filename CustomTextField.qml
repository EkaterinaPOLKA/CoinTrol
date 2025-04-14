import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

TextField {
    id: customTextField
    font.pixelSize: 24
    font.bold: false
    color: color_black
    font.family: "Arial"
    validator: RegularExpressionValidator {
        regularExpression: /^(,)?(0)?([0-9]+(,[0-9]{0,2})?)?$/
    }
    property string previousText: ""
    onTextChanged: {
        if (text.length > 0) {
            if (text.startsWith(",")) {
                text = "0," + text.slice(1);
            }
            let commaCount = (text.match(/,/g) || []).length;
            if (commaCount > 1) {
                text = text.replace(/,/g, "");
                text = text.slice(0, text.lastIndexOf(",")) + ",";
            }
            let parts = text.split(",");
            if (parts.length > 1 && parts[1].length > 2) {
                parts[1] = parts[1].slice(0, 2);
            }
            text = parts.join(",");
            if (text.length > 1 && text.startsWith("0") && text.charAt(1) !== ",") {
                text = text.slice(1);
            }
            if (previousText !== text) {
                previousText = text;
                forceActiveFocus();
            }
        }
    }
    onAccepted: {
        if (text.length > 0) {
            if (text.startsWith(",")) {
                text = "0," + text.slice(1);
            }
            let commaCount = (text.match(/,/g) || []).length;
            if (commaCount > 1) {
                text = text.replace(/,/g, "");
                text = text.slice(0, text.lastIndexOf(",")) + ",";
            }
            let parts = text.split(",");
            if (parts.length > 1 && parts[1].length > 2) {
                parts[1] = parts[1].slice(0, 2);
            }
            text = parts.join(",");
            if (text.length > 1 && text.startsWith("0") && text.charAt(1) !== ",") {
                text = text.slice(1);
            }
            if (previousText !== text) {
                previousText = text;
                forceActiveFocus();
            }
        }
    }
}
