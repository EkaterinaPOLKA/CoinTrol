import QtQuick
import QtQuick.Controls.Basic

Rectangle {
    id: root
    width: 230
    height: 278
    color: color_white
    border.width: 1
    z: 10

    signal clicked(date date);
    property date selectedDate: new Date()
    property int currentYear: selectedDate.getFullYear()
    property int currentMonth: selectedDate.getMonth()
    property date today: new Date()

    onVisibleChanged: {
        if(visible) list.updateView(selectedDate.getFullYear(), selectedDate.getMonth())
    }

    ListView {
        id: list
        anchors.fill: parent
        anchors.margins: 5
        snapMode: ListView.SnapOneItem
        orientation: Qt.Horizontal
        clip: true
        model: 3000 * 12

        function updateView(year, month) {
            currentYear = year;
            currentMonth = month;
            positionViewAtIndex((year - 1970) * 12 + month, ListView.Center);
        }

        delegate: Item {
            id: itemDate
            width: list.width
            height: list.height
            property int year: Math.floor(index / 12) + 1970
            property int month: index % 12
            property int firstDay: new Date(itemDate.year, itemDate.month, 1).getDay()
            property int daysInMonth: new Date(itemDate.year, itemDate.month + 1, 0).getDate()
            property int prevMonthDays: new Date(itemDate.year, itemDate.month, 0).getDate()
            property int startOffset: (itemDate.firstDay === 0 ? 6 : itemDate.firstDay - 1)

            Column {
                anchors.fill: parent
                spacing: 5

                Rectangle {
                    id: rect_date
                    width: parent.width
                    height: 35
                    color: "transparent"

                    Button {
                        width: 35
                        height: 35
                        text: "<"
                        font.pixelSize: 16
                        background: Rectangle {
                            anchors.fill: parent
                            color: "transparent"
                        }
                        anchors.left: parent.left
                        onClicked: list.updateView(itemDate.month === 0 ? itemDate.year - 1 : itemDate.year, (itemDate.month + 11) % 12);
                    }

                    CustomText14 {
                        text: ['Январь', 'Февраль', 'Март', 'Апрель', 'Май', 'Июнь', 'Июль', 'Август', 'Сентябрь', 'Октябрь', 'Ноябрь', 'Декабрь'][itemDate.month] + ' ' + itemDate.year
                        font.pixelSize: 18
                        font.bold: true
                        anchors.centerIn: parent
                        color: color_black
                    }

                    Button {
                        width: 35
                        height: 35
                        text: ">"
                        font.pixelSize: 16
                        background: Rectangle {
                            anchors.fill: parent
                            color: "transparent"
                        }
                        anchors.right: parent.right
                        visible: !(itemDate.year === today.getFullYear() && itemDate.month === today.getMonth())
                        onClicked: list.updateView(itemDate.month === 11 ? itemDate.year + 1 : itemDate.year, (itemDate.month + 1) % 12);
                    }
                }

                Grid {
                    id: grid
                    width: parent.width
                    height: parent.height * 0.8
                    columns: 7
                    rows: 7
                    property real cellSize: Math.min(width / columns, height / rows)

                    Repeater {
                        model: grid.columns * grid.rows

                        delegate: Rectangle {
                            id: rectDate
                            property int dayIndex: index - 7
                            property int date: rectDate.dayIndex - itemDate.startOffset + 1
                            property bool isPrevMonth: date <= 0
                            property bool isNextMonth: date > itemDate.daysInMonth
                            property bool isCurrentMonth: !rectDate.isPrevMonth && !rectDate.isNextMonth
                            property int displayDate: rectDate.isPrevMonth ? itemDate.prevMonthDays + rectDate.date : (rectDate.isNextMonth ? rectDate.date - itemDate.daysInMonth : rectDate.date)
                            property int displayMonth: rectDate.isPrevMonth ? (itemDate.month === 0 ? 11 : itemDate.month - 1) : (rectDate.isNextMonth ? (itemDate.month === 11 ? 0 : itemDate.month + 1) : itemDate.month)
                            property int displayYear: rectDate.isPrevMonth && itemDate.month === 0 ? itemDate.year - 1 : (rectDate.isNextMonth && itemDate.month === 11 ? itemDate.year + 1 : itemDate.year)

                            width: grid.cellSize
                            height: grid.cellSize
                            border.width: (new Date(rectDate.displayYear, rectDate.displayMonth, rectDate.displayDate).toDateString() === selectedDate.toDateString() && nowDateText.text !== ['Пн', 'Вт', 'Ср', 'Чт', 'Пт', 'Сб', 'Вс'][index]) ? 1 : 0
                            border.color: (new Date(rectDate.displayYear, rectDate.displayMonth, rectDate.displayDate) > today) ? "red" : color_black // Красная рамка, если дата позже текущей
                            radius: 5
                            opacity: !mouseArea.pressed ? 1 : 0.5
                            color: (rectDate.isCurrentMonth || nowDateText.text === ['Пн', 'Вт', 'Ср', 'Чт', 'Пт', 'Сб', 'Вс'][index]) ? color_white : Qt.rgba(0.8, 0.8, 0.8, 0.5)

                            CustomText14 {
                                id: nowDateText
                                anchors.centerIn: parent
                                font.pixelSize: 0.5 * parent.height
                                font.bold: (new Date(rectDate.displayYear, rectDate.displayMonth, rectDate.displayDate).toDateString() === new Date().toDateString() || nowDateText.text === ['Пн', 'Вт', 'Ср', 'Чт', 'Пт', 'Сб', 'Вс'][index])
                                text: rectDate.dayIndex < 0 ? ['Пн', 'Вт', 'Ср', 'Чт', 'Пт', 'Сб', 'Вс'][index] : rectDate.displayDate
                                color: rectDate.isCurrentMonth ? color_black : color_dark
                            }

                            MouseArea {
                                id: mouseArea
                                anchors.fill: parent
                                enabled: rectDate.dayIndex >= 0
                                onClicked: {
                                    let selectedDate = new Date(rectDate.displayYear, rectDate.displayMonth, rectDate.displayDate);
                                    if (selectedDate <= today) {
                                        root.selectedDate = selectedDate;
                                        root.clicked(root.selectedDate);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
            list.updateView(selectedDate.getFullYear(), selectedDate.getMonth())
            fetchExchangeRate(selectedDate)
        }
    }
}
