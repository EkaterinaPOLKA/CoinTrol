import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle {
    id: menu
    height: parent.height
    width: 400
    color: color_background_menu
    border.color: color_black

    property var expenseCategories: []
    property var expenseData: []
    property int currentWalletIndex: 0
    property double sumExpenses: 0.00
    property bool isMusicPlay: false

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
            leftBtn.visible = walletCount > 0;
            rightBtn.visible = walletCount > 0;
            walletsContainer.visible = walletCount >= 0;
        }
        function onUpdateExpenseData() {
            expenseData = {};
            let totalExpenses = 0;
            const transactions = transactionModel.getTransactionsByUser(dbManager.userId);

            const expenseTransactions = transactions.filter(tr => tr.type === "Расход" && tr.status === "Выполнено");

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
                expenseCategories = expenseCategories.concat(categoriesSorted.slice(0, 3));

                if (totalExpenses === 0) {
                    expenseCategories = [];
                }
            } else {
                console.warn("Категории расходов отсутствуют в базе данных для данного пользователя.");
                expenseCategories = [];
            }

            noExpensesMessage.visible = expenseCategories.length === 0;
            rashods.visible = expenseCategories.length > 0;
        }
    }

    function getIconForCategory(category) {
        return categoryIcons[category] || "../Source/icons/other_expenses.svg";
    }

    function getCategoryFromTransaction(transaction) {
        return transaction.category;
    }

    Image {
        id: logo
        anchors.top: parent.top
        anchors.topMargin: 10
        width: parent.width
        height: 198
        source: imageSource
        Component.onCompleted: {
            const currentPalette = dbManager.colorPalette();
            if (currentPalette === "your") {
                source = "../Source/LogoUniversal.png"
            } else {
                source = currentPalette === "green" ? "../Source/LogoG.png" :
                currentPalette === "brown" ? "../Source/LogoBr.png" :
                currentPalette === "blue" ? "../Source/LogoBl.png" : "../Source/LogoUniversal.png";
            }
        }
    }

    Column {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        spacing: 10

        Button {
            id: control
            width: 40
            height: 40
            background: Rectangle {
                color: (control.hovered || control.pressed) ? color_kard_kategory : color_transparent_button
                border.color: color_black
                anchors.fill: parent
                radius: 10
                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: -5
                    Image {
                        width: 13
                        height: 24
                        source: "../Source/Str_left.svg"
                    }
                    Image {
                        width: 13
                        height: 24
                        source: "../Source/Str_left.svg"
                    }
                }
            }

            onClicked: {
                isMenu = false
            }
        }
        Button {
            id: hint
            width: 40
            height: 40
            background: Rectangle {
                color: (hint.hovered || hint.pressed) ? color_kard_kategory : color_transparent_button
                border.color: color_black
                anchors.fill: parent
                radius: 10
                Image {
                    width: 13
                    height: 24
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "../Source/Hint.svg"
                }
            }
            onClicked: {
                isHint = true
            }
        }
    }

    CustomText24 {
        id: namePerson
        anchors.top: logo.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        text: dbManager ? (dbManager.lastName + " " + dbManager.firstName) : "Загрузка..."
    }

    Row {
        id: kards
        anchors.top: namePerson.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 15

        CustomButtonLeft {
            id: leftBtn
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if (currentWalletIndex > 0) {
                    currentWalletIndex--;
                } else {
                    currentWalletIndex = walletModel.count;
                }
            }
        }

        Item {
            id: walletsContainer
            width: 260
            height: 200

            Row {
                spacing: 70

                Repeater {
                    id: repeaterModel
                    model: walletModel.count + 1
                    delegate: Rectangle {
                        width: 260
                        height: 200
                        visible: index === currentWalletIndex
                        color: color_kard_kategory
                        border.color: color_black
                        radius: 10
                        CustomText34 {
                            id: nameWallet
                            anchors.top: parent.top
                            anchors.topMargin: 15
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: color_background_main
                            visible: index !== walletModel.count
                            text: index !== walletModel.count ? "-- " + walletModel.get(index).wallet_name + " --" : ""
                        }
                        Button {
                            width: 15
                            height: 15
                            anchors.right: parent.right
                            anchors.rightMargin: 15
                            anchors.verticalCenter: nameWallet.verticalCenter
                            visible: index !== walletModel.count
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
                            onClicked: {
                                if(index !== walletModel.count){
                                    deleteWdialog = true
                                    deleteWallet.nameWallet = walletModel.get(index).wallet_name
                                }
                            }
                        }
                        CustomText24 {
                            anchors.top: parent.top
                            anchors.topMargin: 10
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: color_background_main
                            visible: index === walletModel.count
                            text: "Добавить кошелек"
                        }
                        CustomText34 {
                            id: summa
                            anchors.centerIn: parent
                            color: color_background_main
                            text: index !== walletModel.count ? walletModel.get(index).balance.toFixed(2).replace(".", ",") + " " + symbolValute : ""
                        }
                        Column {
                            id: text_money
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 10
                            anchors.left: parent.left
                            anchors.leftMargin: 9
                            spacing: 5
                            visible: index !== walletModel.count
                            CustomText14 {
                                color: color_background_main
                                text: "Доходы в этом месяце:"
                            }
                            CustomText14 {
                                color: color_background_main
                                text: "Расходы в этом месяце:"
                            }
                        }
                        Column {
                            id: money
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 10
                            anchors.right: parent.right
                            anchors.rightMargin: 9
                            spacing: 5
                            visible: index !== walletModel.count
                            CustomText14 {
                                color: color_background_main
                                anchors.right: parent.right
                                text: index < walletModel.count ? walletModel.get(index).income.toFixed(2).replace(".", ",") + " " + symbolValute : "0,00 " + symbolValute
                            }
                            CustomText14 {
                                color: color_background_main
                                anchors.right: parent.right
                                text: index < walletModel.count ? walletModel.get(index).expense.toFixed(2).replace(".", ",") + " " + symbolValute : "0,00 " + symbolValute
                            }
                        }
                        Column {
                            visible: index === walletModel.count
                            anchors.centerIn: parent
                            spacing: 15
                            CustomText24 {
                                font.pixelSize: 16
                                text: "Введите 4 цифры"
                                color: color_background_main
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            Rectangle {
                                width: 150
                                height: 32
                                color: color_white
                                border.color: color_black
                                anchors.horizontalCenter: parent.horizontalCenter
                                TextField {
                                    id: customTextField
                                    font.pixelSize: 16
                                    color: color_black
                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                    anchors.fill: parent
                                    leftPadding: 5
                                    placeholderText: "0000"
                                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                                    background: Rectangle {
                                        color: "transparent"
                                        border.width: 0
                                    }
                                    validator: RegularExpressionValidator {
                                        regularExpression: /^[0-9]{0,4}$/
                                    }
                                    property string previousText: ""

                                    onTextChanged: {
                                        text = text.replace(/[^0-9]/g, "");
                                        if (previousText !== text) {
                                            if (text.length > 4) {
                                                text = text.slice(0, 4);
                                            }
                                            previousText = text;
                                            forceActiveFocus();
                                        }
                                    }
                                    onAccepted: {
                                        if (text.length < 4) {
                                            text = text.padStart(4, '0');
                                        }
                                        previousText = text;
                                    }
                                }
                            }
                        }
                        CustomText14 {
                            id: addWarning
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: addBtn.verticalCenter
                            visible: index === walletModel.count
                            text: ""
                            color: "red"
                        }
                        CustomButtonAdd {
                            id: addBtn
                            visible: index === walletModel.count
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.margins: 10
                            onClicked: {
                                if (customTextField.text.length !== 4) {
                                    addWarning.text = "Введите 4 цифры";
                                    return;
                                }

                                const walletId = customTextField.text;
                                const wallets = dbManager.getWalletsByUser(dbManager.userId);
                                var music = isMusicPlay
                                if (wallets.some(wallet => wallet.wallet_name === walletId)) {
                                    addWarning.text = "Такой уже есть";
                                    return;
                                }

                                if (dbManager.createWallet(dbManager.userId, walletId)) {
                                    customTextField.text = "";
                                } else {
                                    addWarning.text = "Не удалось добавить кошелек";
                                }
                            }
                        }

                    }
                }
            }
        }

        CustomButtonRight {
            id: rightBtn
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                currentWalletIndex = (currentWalletIndex + 1) % (walletModel.count + 1);
            }
        }
    }

    CustomText24 {
        id: text_rashods
        text: "Расходы за все время"
        anchors.verticalCenter: btnAll.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 15
    }

    Button {
        id: btnAll
        width: 74
        height: 40
        anchors.top: kards.bottom
        anchors.topMargin: 30
        anchors.right: parent.right
        anchors.rightMargin: 15
        visible: expenseCategories.length > 0
        background: Rectangle {
            color: (btnAll.hovered || btnAll.pressed) ? color_kard_kategory : color_transparent_button
            border.color: color_black
            anchors.fill: parent
            radius: 10
            CustomText24 {
                color: color_background_main
                text: "Все"
                anchors.centerIn: parent
            }
        }
        onClicked: {
            isExpenses = true
        }
    }

    CustomText34 {
        id: noExpensesMessage
        text: "У вас нет расходов"
        visible: expenseCategories.length === 0
        color: color_black
        anchors.top: rashods.top
        anchors.topMargin: 100
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Column {
        id: rashods
        spacing: 15
        anchors.top: btnAll.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter

        Grid {
            columns: 2
            spacing: 15
            flow: Grid.FlowLeftToRight

            Repeater {
                id: expenseCategoryRepeater
                model: Math.min(expenseCategories.length, 4)
                delegate: Rectangle {
                    width: 177
                    height: 123
                    color: color_kard_kategory
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
                        color: color_background_main
                        text: expenseCategories[index].percentage.replace(".", ",")
                    }

                    CustomText24 {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: textCategory.top
                        anchors.bottomMargin: 5
                        color: color_background_main
                        text: expenseCategories[index].amount.replace(".", ",")
                    }

                    CustomText24 {
                        id: textCategory
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 10
                        color: color_background_main
                        font.pixelSize: 16
                        text: expenseCategories[index].name
                    }
                }
            }
        }
    }

    Column {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20
        CustomButton {
            text: "Профиль"
            onClicked: {
                vklProfile.visible = true
                vklProfile.z = 2
            }
        }
        CustomButton {
            text: "Выйти"
            onClicked: {
                transactionModel.loadTransactions(dbManager.userId)
                close()
            }
        }
    }
}
