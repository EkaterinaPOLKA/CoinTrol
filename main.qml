import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects
import QtMultimedia

import "./Widget"
import "./Widget/Settings"
import "./Widget/Tranzakcii"
import "./Widget/Targets"
import "./Source"
import "./Source/icons"
import "./Source/Music"
import "./Widget/Plans"
import "./CustomControls" as CC

ApplicationWindow {
    id: main_window
    visible: true
    visibility: "FullScreen" //"Maximized"
    color: color_background_main
    title: "CoinTrol"

    property string dataEdit: ""
    property string timeEdit: ""
    property string statusEdit: ""
    property string typeEdit: ""
    property string sumEdit: ""
    property string walletEdit: ""
    property string categoryEdit: ""
    property string sourceEdit: ""
    property string descriptionEdit: ""

    property string nameTEdit: ""
    property string dateStartTEdit: ""
    property string savedAmountTEdit: ""
    property string amountTEdit: ""
    property string categoryTEdit: ""
    property string descriptionTEdit: ""
    property string source1TEdit: ""
    property string source2TEdit: ""
    property string source3TEdit: ""

    property string login: ""
    property string password: ""

    property string planName: ""
    property string planType: ""
    property string photoUser: ""

    property string symbolValute: "₽"

    property real totalSum: 0

    onSymbolValuteChanged: {
        transactionModel.getValuteAll(dbManager.userId)
    }

    property bool passwordsMatch: true      //Переключатель совпадения паролей при регистрации
    property bool passwordsMatch2: true     //Переключатель совпадения паролей при регистрации 2
    property bool isRegistering: false      //Переключатель входа и регистрации
    property bool loggedIn: false           //Переключатель входа/регистрации и главного меню
    property bool addDialog: false          //Переключатель ДО добавить транзакцию
    property bool editDialog: false         //Переключатель ДО изменения транзакции
    property bool deleteDialogTranz: false  //Переключатель ДО удаления транзакции
    property bool deletePdialog: false      //Переключатель ДО удаления планов
    property bool deleteWdialog: false      //Переключатель ДО удаления кошелька
    property bool addDOTarget: false        //Переключатель ДО добавить цель
    property bool editDOTarget: false       //Переключатель ДО изменения цели
    property bool deleteDialogTarget: false //Переключатель ДО удаления цели
    property bool paramDOTarget: false      //Переключатель ДО параметров достигнутой цели
    property bool analizDialog: false       //Переключатель ДО анализа финансов
    property bool adviceDialog: false       //Переключатель ДО финансовый совет
    property bool deleteData: false         //Переключатель ДО данных по этому пользователю
    property bool isChangedPassword: false  //Переключатель ДО изменения пароля
    property bool isMenu: false             //Переключатель меню
    property bool isTarget: false           //Переключатель меню вкладки Цели
    property bool isHint: false             //Переключатель подсказки
    property bool isExpenses: false         //Переключатель Все расходы
    property bool isNameValid: false        //Переключатель проверки имени
    property bool isSurnameValid: false     //Переключатель проверки фамилии
    property bool isEmailValid: false       //Переключатель проверки эл.почты
    property bool isPhoneValid: false       //Переключатель проверки ном.телефона
    property bool isTwoFactorAuth: false    //Переключатель двойной аутентификации
    property bool isTechnicalSupport: false //Переключатель технической поддержки

    property int selectedIndex: -1 //Индекс выбранной строки в таблице
    property int selectedIndexTarget: -1 //Индекс выбранной строки в таблице

    property string imageSource: "../Source/LogoG.svg"

    property string ipAddress: netManager.getUserIP() //"26.200.28.63"

    property color color_white: "#F7FFF8"
    property color color_background_main: "#F3FFF4"
    property color color_background_block: "#DDF4E6"
    property color color_background_header: "#DAFFDD"
    property color color_background_menu: "#A9DAC6"
    property color color_transparent_button: "#91C3B7"
    property color color_kard_kategory: "#80B4AE"
    property color color_button: "#5F8F94"
    property color color_dark: "#446B77"
    property color color_black: "#2F4858"

    property color color_white_YOU: "#FFFFFF"
    property color color_background_main_YOU: "#FFFFFF"
    property color color_background_block_YOU: "#FFFFFF"
    property color color_background_header_YOU: "#FFFFFF"
    property color color_background_menu_YOU: "#FFFFFF"
    property color color_transparent_button_YOU: "#FFFFFF"
    property color color_kard_kategory_YOU: "#FFFFFF"
    property color color_button_YOU: "#FFFFFF"
    property color color_dark_YOU: "#FFFFFF"
    property color color_black_YOU: "#FFFFFF"

    property var currencyMap: {
        "Российский рубль (₽) - Россия": "RUB",
        "Австралийский доллар (AUD) - Австралия": "AUD",
        "Азербайджанский манат (₼) - Азербайджан": "AZN",
        "Фунт стерлингов (£) - Великобритания": "GBP",
        "Армянских драмов (դր) - Армения": "AMD",
        "Белорусский рубль (BYN) - Беларусь": "BYN",
        "Болгарский лев (лв) - Болгария": "BGN",
        "Бразильский реал (R$) - Бразилия": "BRL",
        "Форинтов (Ft) - Венгрия": "HUF",
        "Донгов (₫) - Вьетнам": "VND",
        "Гонконгский доллар (HK$) - Гонконг": "HKD",
        "Лари (₾) - Грузия": "GEL",
        "Датская крона (kr) - Дания": "DKK",
        "Дирхам ОАЭ (د.إ) - Объединенные Арабские Эмираты": "AED",
        "Доллар США ($) - США": "USD",
        "Евро (€) - Евросоюз": "EUR",
        "Египетских фунтов (ج.م) - Египет": "EGP",
        "Индийских рупий (₹) - Индия": "INR",
        "Рупий (Rp) - Индонезия": "IDR",
        "Тенге (₸) - Казахстан": "KZT",
        "Канадский доллар (C$) - Канада": "CAD",
        "Катарский риал (ر.ق) - Катар": "QAR",
        "Сомов (с) - Кыргызстан": "KGS",
        "Юань (¥) - Китай": "CNY",
        "Молдавских леев (lei) - Молдова": "MDL",
        "Новозеландский доллар (NZ$) - Новая Зеландия": "NZD",
        "Норвежских крон (kr) - Норвегия": "NOK",
        "Злотый (zł) - Польша": "PLN",
        "Румынский лей (lei) - Румыния": "RON",
        "СДР (специальные права заимствования) (XDR) - Международный валютный фонд": "XDR",
        "Сингапурский доллар (S$) - Сингапур": "SGD",
        "Сомони (ЅМ) - Таджикистан": "TJS",
        "Батов (฿) - Таиланд": "THB",
        "Турецких лир (₺) - Турция": "TRY",
        "Новый туркменский манат (TMM) - Туркменистан": "TMT",
        "Узбекских сумов (soʻm) - Узбекистан": "UZS",
        "Гривен (₴) - Украина": "UAH",
        "Чешских крон (Kč) - Чехия": "CZK",
        "Шведских крон (kr) - Швеция": "SEK",
        "Швейцарский франк (CHF) - Швейцария": "CHF",
        "Сербских динаров (дин) - Сербия": "RSD",
        "Рэндов (R) - Южноафриканская Республика": "ZAR",
        "Вон (₩) - Южная Корея": "KRW",
        "Иен (¥) - Япония": "JPY"
    }

    MediaPlayer {
        id: buttonSound
        source: "./Source/Music/money.mp3"
        audioOutput: AudioOutput {}
    }

    ListModel {
        id: walletNameModel
    }

    ListModel {
        id: walletModel
    }

    ListModel {
        id: weekModel
    }

    ListModel {
        id: monthModel
    }

    ListModel {
        id: yearModel
    }

    ListModel {
        id: targetModel
    }

    ListModel {
        id: targetHistoryModel
    }

    ListModel {
        id: supportMessagesModel
    }

    function loadPalette(palette, logo, colors) {
        if (dbManager !== null && dbManager.updatePalette(palette)) {
            imageSource = logo;
            color_white = colors["color_white"];
            color_background_main = colors["color_background_main"];
            color_background_block = colors["color_background_block"];
            color_background_header = colors["color_background_header"];
            color_background_menu = colors["color_background_menu"];
            color_transparent_button = colors["color_transparent_button"];
            color_kard_kategory = colors["color_kard_kategory"];
            color_button = colors["color_button"];
            color_dark = colors["color_dark"];
            color_black = colors["color_black"];

            const currentPalette = dbManager.colorPalette();
            if (currentPalette === "your") {
                color_white_YOU = color_white;
                color_background_main_YOU = color_background_main;
                color_background_block_YOU = color_background_block;
                color_background_header_YOU = color_background_header;
                color_background_menu_YOU = color_background_menu;
                color_transparent_button_YOU = color_transparent_button;
                color_kard_kategory_YOU = color_kard_kategory;
                color_button_YOU = color_button;
                color_dark_YOU = color_dark;
                color_black_YOU = color_black;
            }
        }
    }

    function initializePalette() {
        const currentPalette = dbManager.colorPalette();
        if (currentPalette === "your") {
            const colors = dbManager.getUserColorPalette(dbManager.userId);
            loadPalette(currentPalette, "../Source/LogoUniversal.png", colors);
        } else {
            const logo = currentPalette === "green" ? "../Source/LogoG.png" :
                         currentPalette === "brown" ? "../Source/LogoBr.png" :
                         currentPalette === "blue" ? "../Source/LogoBl.png" : "../Source/LogoUniversal.png";

            const colors = dbManager.getStandardPalette(currentPalette);
            loadPalette(currentPalette, logo, colors);
        }
    }

    function stringToAmount(value) {
        return value.replace(".", ",");
    }

    function updateCategoryModel(catTranz, comboBox) {
        Qt.callLater(function () {
            var categories = [];
            if (catTranz === 0) {  // 0 - индекс для "Расход"
                categories = [
                    "Жилище и ком. услуги",
                    "Транспорт",
                    "Продукты питания",
                    "Здоровье и медицина",
                    "Одежда и обувь",
                    "Развлечения и досуг",
                    "Образование",
                    "Страхование",
                    "Связь",
                    "Подарки и благотворит.",
                    "Прочие расходы",
                    "Кредит",
                    "Ипотека",
                    "Автомобиль",
                    "Цель",
                    "Услуги",
                    "Путешествия",
                    "Косметика и гигиена",
                    "Спорт и фитнес",
                    "Книги",
                    "Гаджеты",
                    "Домашние животные",
                    "Хобби",
                    "Товары для дома",
                    "Культура",
                    "Сборы",
                    "Налоги"
                ];
            } else if (catTranz === 1) {  // 1 - индекс для "Доход"
                categories = [
                    "Заработная плата",
                    "Бонусы и премии",
                    "Дополнительный доход",
                    "Дивиденды",
                    "Проценты по вкладам",
                    "Возврат налогов",
                    "Подарки",
                    "Прочие доходы",
                    "Находка",
                    "Продажа активов",
                    "Аренда",
                    "Инвестиции",
                    "Займ",
                    "Фриланс",
                    "Онлайн-бизнес",
                    "Компенсации и возмещения",
                    "Стипендия",
                    "Пенсия",
                    "Дотации и субсидии",
                    "Прибыль от партнерств",
                    "Авторские гонорары"
                ];
            }

            if (comboBox) {
                comboBox.model = categories;
                if (categoryEdit && editDialog) {  // Установим индекс категории, если уже редактируется категория
                    for (var i = 0; i < categories.length; i++) {
                        if (categories[i] === categoryEdit) {
                            comboBox.currentIndex = i;
                            break;
                        }
                    }
                }
            }
        });
    }

    function loadPlans(type) {
        const plans = transactionModel.fetchPlans(dbManager.userId, type);
        const model = type === "Неделя" ? weekModel : (type === "Месяц" ? monthModel : yearModel);
        model.clear();
        if (plans.length > 0) {
            model.append(plans);
        }
    }

    function loadTargets(){
        const targets = transactionModel.fetchTargets(dbManager.userId);
        targetModel.clear();
        if (targets.length > 0) {
            targetModel.append(targets);
        }
    }

    function loadTargetHistory(){
        const targets = transactionModel.fetchTargetsHistory(dbManager.userId);
        targetHistoryModel.clear();
        if (targets.length > 0) {
            targetHistoryModel.append(targets);
        }
    }

    function updateTargetDisplay() {
        if (targetHistoryModel.count > 0) {
            for(var i = 0; i < targetHistoryModel.count; i++){
                const target = targetHistoryModel.get(i);
                totalSum += target.saved_amount;
            }
            textSumma.text = "Потрачено в общей сумме: " + totalSum + " " + symbolValute;
        }
    }

    Timer {
        id: inactivityTimer
        interval: 120000 //600000 - 10 минут   //120000 - 2 минуты
        repeat: false
        running: true
        onTriggered: {
            vklProfile.visible = false
            vklProfile.z = 0

            technicalSupport.visible = false
            technicalSupport.z = 0

            bar1.currentIndex = 0

            loggedIn = false
            isMenu = false
            isTarget = false
            isHint = false
            isExpenses = false
            addDialog = false
            editDialog = false
            deleteDialogTranz = false
            deletePdialog = false
            deleteWdialog = false
            addDOTarget = false
            editDOTarget = false
            deleteDialogTarget = false
            paramDOTarget = false
            analizDialog = false
            adviceDialog = false
            deleteData = false
            isChangedPassword = false
            isTwoFactorAuth = false
            isTechnicalSupport = false
            selectedIndex = -1

            color_white = "#F7FFF8"
            color_background_main = "#F3FFF4"
            color_background_block = "#DDF4E6"
            color_background_header = "#DAFFDD"
            color_background_menu = "#A9DAC6"
            color_transparent_button = "#91C3B7"
            color_kard_kategory = "#80B4AE"
            color_button = "#5F8F94"
            color_dark = "#446B77"
            color_black = "#2F4858"

            color_white_YOU = "#FFFFFF"
            color_background_main_YOU = "#FFFFFF"
            color_background_block_YOU = "#FFFFFF"
            color_background_header_YOU = "#FFFFFF"
            color_background_menu_YOU = "#FFFFFF"
            color_transparent_button_YOU = "#FFFFFF"
            color_kard_kategory_YOU = "#FFFFFF"
            color_button_YOU = "#FFFFFF"
            color_dark_YOU = "#FFFFFF"
            color_black_YOU = "#FFFFFF"
        }
    }

    function resetInactivityTimer() {
        inactivityTimer.restart();
    }

    MouseArea {
        id: activityMouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
        z: -1

        onClicked: resetInactivityTimer()
        onReleased: resetInactivityTimer()
        onPressed: resetInactivityTimer()
        onPositionChanged: resetInactivityTimer()
        onHoverEnabledChanged: resetInactivityTimer()
    }

    FocusScope {
        id: keyListener
        focus: true
        Keys.onPressed: {
            resetInactivityTimer();
        }
    }

    TechnicalSupport {
        id: technicalSupport
        visible: false
    }

    CC.CustomDO2FA {
        id: doFA2
    }

    DOdeleteTranzakcii {
        id: deleteTranz
    }

    DOeditTranzakcii {
        id: editTranz
    }

    DOaddTranzakcii {
        id: addTranz
    }

    DOanalizTranzakcii {
        id: analizTranz
    }

    DOadviceTranzakcii {
        id: adviceTranz
    }

    CC.CustomDOexpenses {
        id: expenses
    }

    CC.CustomDOhint {
        id: help
    }

    DOdeletePlans {
        id: deletePlans
    }

    CC.CustomDOwallet {
        id: deleteWallet
    }

    DOdeleteData {
        id: deleteDOData
    }

    DOchangedPassword {
        id: changedDOpassword
    }

    DOaddTarget {
        id: addTarget
    }

    DOeditTarget {
        id: editTarget
    }

    DOdeleteTarget {
        id: deleteTarget
    }

    DOparamTarget {
        id: paramTarget
    }

    Image {
        id: image
        anchors.fill: parent
        anchors.centerIn: parent
        source: "Source/BigLogo.png"
        z: 0
        visible: !loggedIn
    }

    CC.CustomDOlogin {
        id: dialog
    }

    VklProfile {
        id: vklProfile
        visible: false
    }

    CC.CustomMenu {
        id: menuYES
        visible: loggedIn && isMenu
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -1
    }

    Rectangle {
        id: menuNO
        visible: loggedIn && !isMenu
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -1
        width: 60
        color: color_background_menu
        border.color: color_black

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
                    Row{
                        anchors.centerIn: parent
                        spacing: -5
                        Image {
                            width: 13
                            height: 24
                            source: "../Source/Str_right.svg"
                        }
                        Image {
                            width: 13
                            height: 24
                            source: "../Source/Str_right.svg"
                        }
                    }
                }
                onClicked: {
                    isMenu = true
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

        CC.CustomText24 {
            id: namePerson
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: dbManager ? (dbManager.lastName + " " + dbManager.firstName) : "Загрузка..."
            rotation: -90
        }
    }

    Rectangle {
        id: header
        visible: loggedIn
        anchors.top: parent.top
        anchors.left: isMenu ? menuYES.right : menuNO.right
        anchors.leftMargin: -1
        anchors.right: parent.right
        anchors.rightMargin: -1
        height: 67
        color: color_background_header
        border.color: color_black

        TabBar {
            id: bar1
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 20
            spacing: 20
            background: Rectangle {
                color: color_background_header
            }

            CC.CustomTabButton {
                text: "Главная"
                onCheckedChanged: {
                    if(checked){
                        bar1.currentIndex = 0
                    }
                }
            }
            CC.CustomTabButton {
                text: "График"
                onCheckedChanged: {
                    if(checked){
                        bar1.currentIndex = 1
                    }
                }
            }
            CC.CustomTabButton {
                text: "Планы"
                onCheckedChanged: {
                    if(checked){
                        bar1.currentIndex = 2
                    }
                }
            }
            CC.CustomTabButton {
                text: "Цели"
                onCheckedChanged: {
                    if(checked){
                        bar1.currentIndex = 3
                    }
                }
            }
            CC.CustomTabButton {
                text: "Валюты"
                onCheckedChanged: {
                    if(checked){
                        bar1.currentIndex = 4
                    }
                }
            }
            CC.CustomTabButton {
                text: "Настройки"
                onCheckedChanged: {
                    if(checked){
                        bar1.currentIndex = 5
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: header.left
        anchors.leftMargin: 1
        anchors.right: header.right
        color: color_background_main
        visible: loggedIn && bar1.currentIndex === 0

        VklTranzakcii {
            id: vklTranzakcii
            anchors.fill: parent
            anchors.margins: 20
        }
    }

    Rectangle {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: header.left
        anchors.leftMargin: 1
        anchors.right: header.right
        color: color_background_main
        visible: loggedIn && bar1.currentIndex === 1

        VklGraphic {
            id: vklMain
            anchors.fill: parent
            anchors.margins: 20
        }
    }

    Rectangle {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: header.left
        anchors.leftMargin: 1
        anchors.right: header.right
        color: color_background_main
        visible: loggedIn && bar1.currentIndex === 2

        VklPlans {
            id: vklPlans
            anchors.fill: parent
            anchors.margins: 20
        }
    }

    Rectangle {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: header.left
        anchors.leftMargin: 1
        anchors.right: header.right
        color: color_background_main
        visible: loggedIn && bar1.currentIndex === 3

        VklTarget {
            id: vklTarget
            anchors.fill: parent
            anchors.margins: 20
        }
    }

    Rectangle {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: header.left
        anchors.leftMargin: 1
        anchors.right: header.right
        color: color_background_main
        visible: loggedIn && bar1.currentIndex === 4

        VklValuta {
            id: vklValuta
            anchors.fill: parent
            anchors.margins: 20
        }
    }

    Rectangle {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: header.left
        anchors.leftMargin: 1
        anchors.right: header.right
        color: color_background_main
        visible: loggedIn && bar1.currentIndex === 5

        VklSettings {
            id: vklSettings
            anchors.fill: parent
            anchors.margins: 20
        }
    }
}
