import QtQuick.Controls.Basic
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick

Dialog {
    id: fa2
    width: 360
    height: 440
    anchors.centerIn: parent
    background: Rectangle {
        color: color_kard_kategory
        border.color: color_black
        radius: 10
    }
    visible: isTwoFactorAuth
    modal: true
    focus: true
    z: 10
    opacity: 0.95
    closePolicy: Popup.NoAutoClose

    property string enterORadd: ""

    onAccepted: {
        isTwoFactorAuth = false;
    }

    onRejected: {
        isTwoFactorAuth = false;
    }

    function linePathContains(index) {
        for (var i = 0; i < linePath.length; i++) {
            if (linePath[i].index === index)
                return true;
        }
        return false;
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
        z: 15
        onClicked: resetInactivityTimer()
        onPressed: resetInactivityTimer()
        onReleased: resetInactivityTimer()
        onPositionChanged: resetInactivityTimer()
    }

    CustomText34 {
        id: textTranz
        color: color_background_block
        text: "Графический ключ"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        font.bold: true
    }

    Canvas {
        id: lineCanvas
        anchors.fill: parent
        z: 20
        onPaint: {
            var ctx = getContext('2d');
            ctx.clearRect(0, 0, width, height); // Очищаем холст

            // Рисуем линию
            if (linePath.length > 0) {
                ctx.beginPath();
                ctx.moveTo(linePath[0].x, linePath[0].y);
                for (var i = 1; i < linePath.length; i++) {
                    ctx.lineTo(linePath[i].x, linePath[i].y);
                }
                ctx.strokeStyle = 'red';
                ctx.lineWidth = 2;
                ctx.stroke();
            }
            warning.text = ""
            warning.visible = false
        }
    }

    GridLayout {
        id: patternGrid
        columns: 3
        rowSpacing: 20
        columnSpacing: 20
        anchors.centerIn: parent

        Repeater {
            model: 9
            delegate: Rectangle {
                id: dot
                width: 60
                height: 60
                radius: width / 2
                color: color_white
                border.color: color_black
                border.width: 2

                property int dotIndex: model.index

                Rectangle {
                    width: 6
                    height: 6
                    radius: 3
                    color: color_black
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        if (linePath.length < 9 && !linePathContains(dotIndex)) {
                            var centerInCanvas = dot.mapToItem(lineCanvas, dot.width / 2, dot.height / 2);
                            linePath.push({ x: centerInCanvas.x, y: centerInCanvas.y, index: dotIndex });
                            lineCanvas.requestPaint();
                            warning.text = ""
                            warning.visible = false
                        }
                    }
                }
            }
        }
    }

    // Переменные для хранения пути линии
    property var linePath: []

    CustomText14 {
        id: warning
        visible: false
        color: "red"
        anchors.bottom: rowBtn.top
        anchors.bottomMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
        id: rowBtn
        spacing: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        CustomButton {
            text: "Сбросить"
            width: 150
            onClicked: {
                linePath = [];
                lineCanvas.requestPaint();
            }
        }
        CustomButton {
            text: enterORadd === "добавить" ? "Сохранить" : "Войти"
            width: 150
            onClicked: {
                var drawingKey = JSON.stringify(linePath);
                if(enterORadd === "добавить"){
                    dbManager.setDrawing_key(drawingKey);
                } else {
                    if(!dbManager.checkDrawingKey(drawingKey)){
                        warning.text = "Неверный графический ключ"
                        warning.visible = true
                        return;
                    } else {
                        warning.text = ""
                        warning.visible = false
                    }
                }
                linePath = [];
                lineCanvas.requestPaint();
                isTwoFactorAuth = false;
            }
        }
    }
}
