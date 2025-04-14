import QtQuick
import QtQuick.Controls

Rectangle {
    id: dialog
    width: parent.width / 2
    height: parent.height / 1.6
    anchors.centerIn: parent
    color: color_kard_kategory
    border.color: color_black
    radius: 10
    opacity: 0
    visible: !loggedIn
    onVisibleChanged: {
        if(visible) {
            isLanguageDetermined = false
        }
    }

    property string currentLanguage: ""
    property bool isLanguageDetermined: false

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

    function determineLanguage(inputText) {
        if (isLanguageDetermined) return;

        var firstLetter = inputText.match(/[a-zA-Zа-яА-ЯЁ]/);
        if (!firstLetter) {
            return;
        }

        var charCode = firstLetter[0].charCodeAt(0);
        var localLang = "";

        if (charCode >= 0x0400 && charCode <= 0x04FF) {
            localLang = "РУС";
        } else if ((charCode >= 0x0041 && charCode <= 0x005A) || (charCode >= 0x0061 && charCode <= 0x007A)) {
            localLang = "ENG";
        }

        if (localLang) {
            currentLanguage = localLang;
            langDisplay.visible = true;
            langText.text = currentLanguage;
            isLanguageDetermined = true;
        }
    }

    function isPasswordValid(password) {
        const minLen = 8;
        const hasUpperCase = /[A-ZА-ЯЁ]/.test(password);
        const hasLowerCase = /[a-zа-яё]/.test(password);
        const hasNumber = /\d/.test(password);
        const hasSpecialChar = /[!@#$%^&*(),.?":{}|<>]/.test(password);
        const noSequentialChars = !(/(.)\1{2,}/.test(password)); // например: 000, aaa
        const noConsecutiveOrder = !(/^(01234567|12345678|abcdefgh|ABCDEFGH|абвгдеж|АБВГДЕЁЖ|0123456789|00000000|aaaaaaaa|AAAAAAAA)$/).test(password);

        return password.length >= minLen && hasUpperCase && hasLowerCase && hasNumber && hasSpecialChar && noSequentialChars && noConsecutiveOrder;
    }

    Component.onCompleted: {
        langDisplay.visible = currentLanguage;
        langText.text = currentLanguage;
    }

    Keys.onPressed: function(event) {
        if (event.modifiers & Qt.AltModifier && event.modifiers & Qt.ShiftModifier) {
            currentLanguage = (currentLanguage === "РУС") ? "ENG" : "РУС";
            langText.text = currentLanguage;
        }
    }

    SequentialAnimation {
        running: true
        OpacityAnimator {
            target: dialog
            from: 0
            to: 0.9
            duration: 3000
        }
    }

    Rectangle {
        id: langDisplay
        width: 60
        height: 30
        color: color_background_main
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.rightMargin: 20

        CustomText24 {
            id: langText
            anchors.centerIn: parent
            text: currentLanguage
            font.bold: true
        }
    }

    CustomText34 {
        id: headerDO
        color: color_background_main
        text: "Добро пожаловать в Coinтроль!"
        anchors.top: parent.top
        anchors.topMargin: 20
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 40
        wrapMode: Text.WordWrap
        font.bold: true
    }

    CustomText34 {
        id: enter
        color: color_background_main
        text: !isRegistering ? "Вход" : "Регистрация"
        anchors.top: headerDO.bottom
        anchors.topMargin: 30
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 40
        wrapMode: Text.WordWrap
        font.bold: true
    }

    Column {
        id: columnField_one
        visible: !isRegistering
        anchors.centerIn: parent
        spacing: 20
        Row {
            spacing: 20
            CustomText24 {
                color: color_background_main
                anchors.verticalCenter: parent.verticalCenter
                width: 100
                text: "Логин:"
            }
            Rectangle {
                width: 300
                height: 32
                color: color_white
                border.color: color_black
                anchors.verticalCenter: parent.verticalCenter
                CustomTextField_text {
                    id: loginField
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    leftPadding: 5
                    placeholderText: "Введите вашу почту или номер телефона"
                    onAccepted: choicePassword.forceActiveFocus()
                    onTextChanged: {
                        warning_one.visible = false
                        login = text
                        if (text.length > 0 && !isLanguageDetermined) {
                            determineLanguage(text);
                        }
                    }
                }
            }
        }
        Row {
            spacing: 20
            CustomText24 {
                color: color_background_main
                anchors.verticalCenter: parent.verticalCenter
                width: 100
                text: "Пароль:"
            }
            Rectangle {
                width: 300
                height: 32
                color: color_white
                border.color: color_black
                anchors.verticalCenter: parent.verticalCenter
                CustomTextField_text {
                    id: choicePassword
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    leftPadding: 5
                    rightPadding: 40
                    echoMode: TextInput.Password
                    placeholderText: "Введите ваш пароль"
                    onTextChanged: {
                        warning_one.visible = false
                        password = text
                        if (text.length > 0 && !isLanguageDetermined) {
                            determineLanguage(text);
                        }
                    }
                    Keys.onReturnPressed: {
                        loginButton.clicked();
                    }
                }
                Image {
                    id: imageItem
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    source: "../Source/CloseEye.svg"
                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            imageItem.source = "../Source/OpenEye.svg"
                            choicePassword.echoMode = TextInput.Normal
                        }
                        onReleased: {
                            imageItem.source = "../Source/CloseEye.svg"
                            choicePassword.echoMode = TextInput.Password
                        }
                    }
                }
            }
        }
    }

    CustomText14 {
        id: warning_one
        anchors.top: columnField_one.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: columnField_one.horizontalCenter
        visible: false
        color: "red"
    }

    Column {
        visible: !isRegistering
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        CustomText24 {
            text: "Регистрация"
            width: 446
            color: color_background_main
            horizontalAlignment: Text.AlignHCenter
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    warning_one.visible = false
                    loginField.text = "";
                    choicePassword.text = "";
                    isRegistering = true
                }
            }
        }
        // CustomText24 {
        //     text: "Забыли пароль?"
        //     width: 446
        //     color: color_background_main
        //     horizontalAlignment: Text.AlignHCenter
        // }
        Row {
            spacing: 20
            CustomButton {
                text: "Выйти"
                onClicked: {
                    loginField.text = "";
                    choicePassword.text = "";
                    close()
                }
            }
            CustomButton {
                id: loginButton
                text: "Войти"
                onClicked: {
                    if (!loginField.text || !choicePassword.text) {
                        warning_one.text = "Логин и пароль должны быть заполнены"
                        warning_one.visible = true
                    } else {
                        var authResult = dbManager.authenticateUserWithIp(loginField.text, choicePassword.text, ipAddress);
                        if (authResult) {
                            buttonSound.play();
                            loggedIn = true;
                            dbManager.loadUserFromDB(loginField.text);
                            transactionModel.loadTransactions(dbManager.userId);
                            transactionModel.getTransactionsByUser(dbManager.userId);
                            transactionModel.getTransactionCount(dbManager.userId);
                            updateCategoryModel();
                            dbManager.countWalletsByUser(dbManager.userId);
                            dbManager.backgroundMusic();
                            initializePalette();
                            photoUser = dbManager.photo();
                            loginField.text = "";
                            choicePassword.text = "";
                        } else {
                            var warning = dbManager.isIpBanned(ipAddress)
                            if(!warning){
                                warning_one.text = "Неверный логин или пароль"
                                warning_one.visible = true
                            } else {
                                warning_one.text = "Доступ запрещен. IP-адрес заблокирован на 3 минуты"
                                warning_one.visible = true
                            }
                        }
                    }
                }
            }
        }
    }

    //-Вход-----------------------------------<<<

    //-Регистрация---------------------------->>>

    Column {
        id: columnField_two
        visible: isRegistering
        anchors.centerIn: parent
        spacing: 20
        Row {
            spacing: 20
            CustomText24 {
                color: color_background_main
                anchors.verticalCenter: parent.verticalCenter
                width: 250
                text: "Имя:"
            }
            Rectangle {
                width: 300
                height: 32
                color: color_white
                border.color: color_black
                anchors.verticalCenter: parent.verticalCenter
                CustomTextField_text {
                    id: nameField
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    leftPadding: 5
                    placeholderText: "Введите ваше имя"
                    onAccepted: surnameField.forceActiveFocus()
                    onTextChanged: {
                        const namePattern = /^[A-ZА-ЯЁ][a-zA-Zа-яёА-ЯЁ]{2,}$/;
                        if (namePattern.test(text)) {
                            nameField.color = color_black;
                            isNameValid = true;
                        } else {
                            nameField.color = "red";
                            isNameValid = false;
                        }
                        warning_two.visible = false;
                        if (text.length > 0 && !isLanguageDetermined) {
                            determineLanguage(text);
                        }
                    }
                }
            }
        }
        Row {
            spacing: 20
            CustomText24 {
                color: color_background_main
                anchors.verticalCenter: parent.verticalCenter
                width: 250
                text: "Фамилия:"
            }
            Rectangle {
                width: 300
                height: 32
                color: color_white
                border.color: color_black
                anchors.verticalCenter: parent.verticalCenter
                CustomTextField_text {
                    id: surnameField
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    leftPadding: 5
                    placeholderText: "Введите вашу фамилию"
                    onAccepted: emailField.forceActiveFocus()
                    onTextChanged: {
                        const surnamePattern = /^[A-ZА-ЯЁ][a-zA-Zа-яёА-ЯЁ]{2,}$/;
                        if (surnamePattern.test(text)) {
                            surnameField.color = color_black;
                            isSurnameValid = true;
                        } else {
                            surnameField.color = "red";
                            isSurnameValid = false;
                        }
                        warning_two.visible = false;
                        if (text.length > 0 && !isLanguageDetermined) {
                            determineLanguage(text);
                        }
                    }
                }
            }
        }
        Row {
            spacing: 20
            CustomText24 {
                color: color_background_main
                anchors.verticalCenter: parent.verticalCenter
                width: 250
                text: "Эл.почта:"
            }
            Rectangle {
                width: 300
                height: 32
                color: color_white
                border.color: color_black
                anchors.verticalCenter: parent.verticalCenter
                CustomTextField_text {
                    id: emailField
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    leftPadding: 5
                    placeholderText: "Введите вашу электронную почту"
                    onAccepted: phoneField.forceActiveFocus()
                    validator: RegularExpressionValidator {
                        regularExpression: /^[^\s@]+@[^\s@]+\.[^\s@]+$/
                    }
                    onTextChanged: {
                        let emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
                        if (emailPattern.test(text)) {
                            emailField.color = color_black;
                            isEmailValid = true;
                        } else {
                            emailField.color = "red";
                            isEmailValid = false;
                        }
                        warning_two.visible = false;
                        if (text.length > 0 && !isLanguageDetermined) {
                            determineLanguage(text);
                        }
                    }
                }
            }
        }
        Row {
            spacing: 20
            CustomText24 {
                color: color_background_main
                anchors.verticalCenter: parent.verticalCenter
                width: 250
                text: "Номер тел.:"
            }
            Rectangle {
                width: 300
                height: 32
                color: color_white
                border.color: color_black
                anchors.verticalCenter: parent.verticalCenter
                CustomTextField_text {
                    id: phoneField
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    leftPadding: 5
                    placeholderText: "Введите ваш номер телефона"
                    onAccepted: choicePasswordReg.forceActiveFocus()
                    validator: RegularExpressionValidator {
                        regularExpression: /^\d{11}$/
                    }
                    onTextChanged: {
                        let phonePattern = /^\d{11}$/;
                        if (phonePattern.test(text)) {
                            phoneField.color = color_black;
                            isPhoneValid = true;
                        } else {
                            phoneField.color = "red";
                            isPhoneValid = false;
                        }
                        warning_two.visible = false
                        if (text.length > 0 && !isLanguageDetermined) {
                            determineLanguage(text);
                        }
                    }
                }
            }
        }
        Row {
            spacing: 20
            CustomText24 {
                color: color_background_main
                anchors.verticalCenter: parent.verticalCenter
                width: 250
                text: "Пароль:"
            }
            Rectangle {
                width: 300
                height: 32
                color: color_white
                border.color: color_black
                anchors.verticalCenter: parent.verticalCenter
                CustomTextField_text {
                    id: choicePasswordReg
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    leftPadding: 5
                    rightPadding: 40
                    echoMode: TextInput.Password
                    placeholderText: "Введите ваш пароль"
                    onTextChanged: {
                        warning_two.visible = false
                        passwordsMatch = (choicePasswordReg.text === choicePasswordRegDouble.text)

                        if (text.length > 0 && !isLanguageDetermined) {
                            determineLanguage(text);
                        }

                        if (!isPasswordValid(text)) {
                            choicePasswordReg.color = "red";
                            warning_two.text = "Пароль должен содержать минимум 8 символов, включая заглавные буквы, цифры, специальные символы и не может содержать последовательности.";
                            warning_two.visible = true;
                        } else {
                            choicePasswordReg.color = color_black;
                        }
                    }
                    onAccepted: choicePasswordRegDouble.forceActiveFocus()
                }
                Image {
                    id: imageItemReg
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    source: "../Source/CloseEye.svg"
                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            imageItemReg.source = "../Source/OpenEye.svg"
                            choicePasswordReg.echoMode = TextInput.Normal
                        }
                        onReleased: {
                            imageItemReg.source = "../Source/CloseEye.svg"
                            choicePasswordReg.echoMode = TextInput.Password
                        }
                    }
                }
            }
        }
        Row {
            spacing: 20
            CustomText24 {
                color: color_background_main
                anchors.verticalCenter: parent.verticalCenter
                width: 250
                text: "Подтвердите пароль:"
            }
            Rectangle {
                width: 300
                height: 32
                color: color_white
                border.color: color_black
                anchors.verticalCenter: parent.verticalCenter
                CustomTextField_text {
                    id: choicePasswordRegDouble
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    leftPadding: 5
                    rightPadding: 40
                    echoMode: TextInput.Password
                    placeholderText: "Введите ваш пароль еще раз"
                    onTextChanged: {
                        warning_two.visible = false
                        passwordsMatch = choicePasswordReg.text === "" || choicePasswordReg.text === choicePasswordRegDouble.text;

                        if (text.length > 0 && !isLanguageDetermined) {
                            determineLanguage(text);
                        }

                        if (!passwordsMatch) {
                            choicePasswordRegDouble.color = "red";
                            warning_two.text = "Пароли не совпадают";
                            warning_two.visible = true;
                        } else {
                            choicePasswordRegDouble.color = color_black;
                        }
                    }
                    color: passwordsMatch ? color_black : "red"
                    Keys.onReturnPressed: {
                        registerButton.clicked();
                    }
                }
                Image {
                    id: imageItemRegDouble
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    source: "../Source/CloseEye.svg"
                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            imageItemRegDouble.source = "../Source/OpenEye.svg"
                            choicePasswordRegDouble.echoMode = TextInput.Normal
                        }
                        onReleased: {
                            imageItemRegDouble.source = "../Source/CloseEye.svg"
                            choicePasswordRegDouble.echoMode = TextInput.Password
                        }
                    }
                }
            }
        }
    }

    CustomText14 {
        id: warning_two
        anchors.top: columnField_two.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: columnField_two.horizontalCenter
        width: parent.width - 20
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        visible: false
        color: "red"
    }

    Column {
        visible: isRegistering
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        CustomText24 {
            text: "Назад"
            width: 253
            color: color_background_main
            horizontalAlignment: Text.AlignHCenter
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    nameField.text = "";
                    surnameField.text = "";
                    emailField.text = "";
                    phoneField.text = "";
                    choicePasswordReg.text = "";
                    choicePasswordRegDouble.text = "";
                    warning_two.visible = false
                    isRegistering = false
                }
            }
        }
        CustomButton {
            id: registerButton
            text: "Зарегистрироваться"
            width: 253
            onClicked: {
                if (!nameField.text || !surnameField.text || !emailField.text || !phoneField.text || !choicePasswordReg.text || !choicePasswordRegDouble.text) {
                    warning_two.text = "Все поля должны быть заполнены"
                    warning_two.visible = true
                } else if (!isNameValid || !isSurnameValid){
                    warning_two.text = "Имя и фамилия должны содержать минимум 3 буквы, первая заглавная."
                    warning_two.visible = true;
                } else if (!isEmailValid) {
                    warning_two.text = "Введите корректный адрес электронной почты"
                    warning_two.visible = true
                } else if (!isPhoneValid) {
                    warning_two.text = "Введите корректный номер телефона"
                    warning_two.visible = true
                } else if (!passwordsMatch) {
                    warning_two.text = "Пароли не совпадают"
                    warning_two.visible = true
                } else {
                    if (dbManager.registerUser(nameField.text, surnameField.text, emailField.text, phoneField.text, choicePasswordReg.text, "Россия", "Российский рубль (₽)", "green", "24", "0", "0", "", "")) {
                        buttonSound.play();
                        dbManager.loadUserFromDB(emailField.text);
                        transactionModel.loadTransactions(emailField.text);
                        updateCategoryModel();
                        dbManager.countWalletsByUser(dbManager.userId);
                        nameField.text = "";
                        surnameField.text = "";
                        emailField.text = "";
                        phoneField.text = "";
                        choicePasswordReg.text = "";
                        choicePasswordRegDouble.text = "";
                        loggedIn = true;
                        isRegistering = false;
                    } else {
                        warning_two.text = "Регистрация не удалась"
                        warning_two.visible = true
                    }
                }
            }
        }
    }

    //-Регистрация----------------------------<<<
}
