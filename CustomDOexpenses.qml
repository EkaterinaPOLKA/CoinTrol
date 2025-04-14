import QtQuick.Controls.Basic
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick

Dialog {
    id: expenses
    width: Math.min(parent.width / 2, 600)
    height: Math.max(parent.height / 1.6, 780)
    anchors.centerIn: parent
    background: Rectangle {
        color: color_kard_kategory
        border.color: color_black
        radius: 10
    }
    visible: isExpenses
    modal: true
    focus: true
    z: 10
    opacity: 0.9

    onAccepted: {
        isExpenses = false;
    }
    onRejected: {
        isExpenses = false;
    }

    Connections {
        target: dbManager
        function onWalletCountChanged(walletCount, userId) {
            if (userId > 0) {
                walletModel.clear();
                const wallets = dbManager.getWalletsByUser(userId);
                for (let i = 0; i < wallets.length; i++) {
                    const incomeExpenses = dbManager.getMonthlyIncomeExpenses(wallets[i].wallet_name);
                    walletModel.append({
                                           wallet_name: wallets[i].wallet_name,
                                           balance: wallets[i].balance,
                                           income: incomeExpenses["income"] || 0,
                                           expense: incomeExpenses["expense"] || 0
                                       });
                }
            } else {
                console.error("Invalid userId:", userId);
            }
        }
        function onUpdateExpenseData() {
            expenseData = {};
            let totalExpenses = 0;
            const transactions = transactionModel.getTransactionsByUser(dbManager.userId);
            const expenseTransactions = transactions.filter(tr => tr.type === "Расход");

            for (let tr of expenseTransactions) {
                const category = getCategoryFromTransaction(tr);
                expenseData[category] = (expenseData[category] || 0) + tr.amount;
            }

            totalExpenses = Object.values(expenseData).reduce((a, b) => a + b, 0);
            sumExpenses = totalExpenses.toFixed(2);

            const categoryQuery = dbManager.getCategoriesByUser(dbManager.userId);
            const categoryData = {};

            if (categoryQuery.length > 0) {
                for (let row of categoryQuery) {
                    const name = row.group_name;
                    const value = row.value;
                    categoryData[name] = value;
                }

                expenseCategories = [
                            {
                                name: "Все расходы",
                                amount: totalExpenses > 0 ? totalExpenses.toFixed(2) + " " + symbolValute : "0.00 " + symbolValute,
                                percentage: totalExpenses > 0 ? "" : "0.00%",
                                icon: "../Source/icons/allRashod.svg"
                            }
                        ];

                const categoriesSorted = Object.keys(categoryData).map(category => {
                                                                           const amount = categoryData[category] || 0;
                                                                           const percentage = totalExpenses > 0 ? (amount / totalExpenses * 100) : 0;
                                                                           return {
                                                                               name: category,
                                                                               amount: amount.toFixed(2) + " " + symbolValute,
                                                                               percentage: percentage.toFixed(2) + "%",
                                                                               icon: getIconForCategory(category)
                                                                           };
                                                                       });

                categoriesSorted.sort((a, b) => parseFloat(b.percentage) - parseFloat(a.percentage));

                expenseCategories = expenseCategories.concat(categoriesSorted);

                if (totalExpenses === 0) {
                    expenseCategories = [];
                }

            } else {
                expenseCategories = [];
            }

            noExpensesMessage.visible = expenseCategories.length === 0;
            rashods.visible = expenseCategories.length > 0;
        }
    }

    property var expenseCategories: []
    property var expenseData: []
    property double sumExpenses: 0.00
    // property bool isDatePicker: false

    property var categoryIcons: {
        "Все расходы": "../Source/icons/allRashod.svg",
        "Жилище и ком. услуги": "../Source/icons/housing_and_utility_services.svg",
        "Транспорт": "../Source/icons/transportation.svg",
        "Продукты питания": "../Source/icons/food_products.svg",
        "Здоровье и медицина": "../Source/icons/health_and_medicine.svg",
        "Одежда и обувь": "../Source/icons/clothing_and_footwear.svg",
        "Развлечения и досуг": "../Source/icons/entertainment_and_leisure.svg",
        "Образование": "../Source/icons/education.svg",
        "Страхование": "../Source/icons/insurance.svg",
        "Связь": "../Source/icons/communication.svg",
        "Подарки и благотворит.": "../Source/icons/gifts_and_charity.svg",
        "Прочие расходы": "../Source/icons/other_expenses.svg",
        "Кредит": "../Source/icons/credit.svg",
        "Ипотека": "../Source/icons/mortgage.svg",
        "Автомобиль": "../Source/icons/automobile.svg",
        "Цель": "../Source/icons/goal.svg",
        "Услуги": "../Source/icons/services.svg",
        "Путешествия": "../Source/icons/travel.svg",
        "Косметика и гигиена": "../Source/icons/cosmetics_and_hygiene.svg",
        "Спорт и фитнес": "../Source/icons/sports_and_fitness.svg",
        "Книги": "../Source/icons/books.svg",
        "Гаджеты": "../Source/icons/gadgets.svg",
        "Домашние животные": "../Source/icons/pets.svg",
        "Хобби": "../Source/icons/hobbies.svg",
        "Товары для дома": "../Source/icons/home_goods.svg",
        "Культура": "../Source/icons/culture.svg",
        "Сборы": "../Source/icons/fees.svg",
        "Налоги": "../Source/icons/taxes.svg"
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

    function getIconForCategory(category) {
        return categoryIcons[category] || "../Source/icons/other_expenses.svg";
    }

    function getCategoryFromTransaction(transaction) {
        return transaction.category;
    }

    // CustomDatePicker {
    //     visible: isDatePicker
    //     onClicked: {
    //         console.log('onClicked', Qt.formatDate(date, 'M/d/yyyy'))
    //     }
    // }

    // CC.CustomText24 {
    //     text: "Дата и время:"
    //     topPadding: 5
    //     width: 150
    //     color: color_background_main
    //     anchors.verticalCenter: parent.verticalCenter
    // }
    // Rectangle {
    //     width: 150
    //     height: 32
    //     color: "white"
    //     border.color: dataTranz.palette.mid
    //     anchors.verticalCenter: parent.verticalCenter

    //     CC.CustomText14 {
    //         id: dataTranz
    //         font.pixelSize: 16
    //         horizontalAlignment: Text.AlignLeft
    //         verticalAlignment: Text.AlignVCenter
    //         anchors.fill: parent
    //         leftPadding: 5
    //         text: Qt.formatDateTime(new Date(), "dd.MM.yyyy")
    //     }
    // }
    // Image {
    //     width: 28
    //     height: 28
    //     source: "../../Source/KalendarW.svg"
    //     anchors.verticalCenter: parent.verticalCenter
    // }

    CustomText34 {
        id: textTranz
        color: color_background_block
        text: "Все расходы"
        anchors.top: parent.top
        anchors.topMargin: 20
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 40
        wrapMode: Text.WordWrap
        font.bold: true
    }

    Button {
        id: control
        width: 30
        height: 30
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.verticalCenter: textTranz.verticalCenter
        background: Rectangle {
            anchors.fill: parent
            anchors.centerIn: parent
            color: "transparent"
            Image {
                anchors.fill: parent
                anchors.centerIn: parent
                source: "../Source/Close.svg"
            }
        }
        onClicked: isExpenses = false
    }

    CustomText34 {
        id: noExpensesMessage
        text: "У вас нет расходов"
        visible: expenseCategories.length === 0
        color: color_black
        anchors.centerIn: parent
    }

    Flickable {
        anchors.top: textTranz.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: parent.height - textTranz.height - 50
        contentHeight: Math.ceil(expenseCategories.length / 3) * 138
        clip: true
        Column {
            id: rashods
            spacing: 15
            width: parent.width

            Grid {
                columns: 3
                spacing: 15
                flow: Grid.FlowLeftToRight

                Repeater {
                    id: expenseCategoryRepeater
                    model: expenseCategories.length
                    delegate: Rectangle {
                        width: 177
                        height: 123
                        color: color_background_menu
                        border.color: color_black
                        radius: 10

                        Image {
                            id: imageRashods
                            anchors.top: parent.top
                            anchors.topMargin: 10
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            source: expenseCategories[index].icon
                        }

                        CustomText24 {
                            anchors.top: parent.top
                            anchors.topMargin: 15
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            color: color_black
                            text: expenseCategories[index].percentage.replace(".", ",")
                        }

                        CustomText24 {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: textCategory.top
                            anchors.bottomMargin: 5
                            color: color_black
                            text: expenseCategories[index].amount.replace(".", ",")
                        }

                        CustomText24 {
                            id: textCategory
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 10
                            color: color_black
                            font.pixelSize: 16
                            text: expenseCategories[index].name
                        }
                    }
                }
            }
        }
    }
}
