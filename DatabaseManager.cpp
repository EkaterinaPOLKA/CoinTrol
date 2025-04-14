#include "DatabaseManager.h"
#include "TransactionModel.h"

#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariantMap>
#include <QVariantList>
#include <QCryptographicHash>
#include <QDate>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), m_transactionModel(nullptr) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("app_database.db");

    if (openDatabase()) {
        createTables();
    } else {
        qDebug() << "Не удалось открыть базу данных!";
    }
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
    delete m_transactionModel;
}

bool DatabaseManager::createTables() {
    struct TableCreation {
        QString name;
        QString query;
    };

    QVector<TableCreation> tables = {
        {"users", "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, first_name TEXT NOT NULL, last_name TEXT NOT NULL, email TEXT NOT NULL UNIQUE, phone TEXT NOT NULL UNIQUE, password TEXT NOT NULL, country TEXT DEFAULT 'Россия', currency TEXT DEFAULT 'Российский рубль (₽)', color_palette TEXT DEFAULT 'green', font_size INTEGER DEFAULT 24, background_music INTEGER DEFAULT 1, two_factor_auth INTEGER DEFAULT 0, photo TEXT, drawing_key TEXT);"},
        {"transactions", "CREATE TABLE IF NOT EXISTS transactions (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER NOT NULL, date_time TEXT NOT NULL, status TEXT NOT NULL, type TEXT NOT NULL, amount REAL NOT NULL, previous_balance REAL NOT NULL, new_balance REAL NOT NULL, wallet TEXT NOT NULL, category TEXT NOT NULL, target TEXT, source TEXT, description TEXT, FOREIGN KEY (user_id) REFERENCES users(id));"},
        {"wallets", "CREATE TABLE IF NOT EXISTS wallets (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER, wallet_name VARCHAR(4), balance REAL DEFAULT 0.0, created_at DATETIME DEFAULT CURRENT_TIMESTAMP, FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE);"},
        {"targets", "CREATE TABLE IF NOT EXISTS targets (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER NOT NULL, date_start TEXT NOT NULL, target_name TEXT NOT NULL, target_amount REAL NOT NULL, saved_amount REAL DEFAULT 0.0, category TEXT NOT NULL, description TEXT, source_1 TEXT, source_2 TEXT, source_3 TEXT, FOREIGN KEY (user_id) REFERENCES users(id));"},
        {"target_history", "CREATE TABLE IF NOT EXISTS target_history (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER NOT NULL, target_id INTEGER NOT NULL, date_start TEXT NOT NULL, target_name TEXT NOT NULL, target_amount REAL NOT NULL, saved_amount REAL DEFAULT 0.0, change_date TEXT NOT NULL, category TEXT NOT NULL, description TEXT, source_1 TEXT, source_2 TEXT, source_3 TEXT, FOREIGN KEY (target_id) REFERENCES targets(id) ON DELETE CASCADE, FOREIGN KEY (user_id) REFERENCES users(id));"},
        {"plans", "CREATE TABLE IF NOT EXISTS plans (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER NOT NULL, type TEXT NOT NULL, name TEXT NOT NULL, amount REAL DEFAULT 0.00, description TEXT, UNIQUE(type, name), FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE);"},
        {"user_colors", "CREATE TABLE IF NOT EXISTS user_colors (user_id INTEGER PRIMARY KEY, color_white TEXT DEFAULT '#FFFFFF', color_background_main TEXT DEFAULT '#F5F5F5', color_background_block TEXT DEFAULT '#D9D9D9', color_background_header TEXT DEFAULT '#808080', color_background_menu TEXT DEFAULT '#C0C0C0', color_transparent_button TEXT DEFAULT '#808080', color_kard_kategory TEXT DEFAULT '#BEBEBE', color_button TEXT DEFAULT '#A0A0A0', color_dark TEXT DEFAULT '#808080', color_black TEXT DEFAULT '#000000', FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE);" },
        {"categories", "CREATE TABLE IF NOT EXISTS categories (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER NOT NULL, wallet_id INTEGER NOT NULL, group_name TEXT NOT NULL, date_time TEXT NOT NULL, link TEXT, value REAL DEFAULT 0.00, percentage REAL DEFAULT 0.0, FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE, FOREIGN KEY (wallet_id) REFERENCES wallets(id) ON DELETE CASCADE, UNIQUE(user_id, wallet_id, group_name));"},
        {"ip_bans", "CREATE TABLE IF NOT EXISTS ip_bans (id INTEGER PRIMARY KEY AUTOINCREMENT, ip_address TEXT UNIQUE, ban_time DATETIME DEFAULT CURRENT_TIMESTAMP, ban_duration INTEGER DEFAULT 15);"},
        {"failed_logins", "CREATE TABLE IF NOT EXISTS failed_logins (ip_address TEXT PRIMARY KEY, attempt_count INTEGER DEFAULT 1);"}
    };

    for (const auto& table : tables) {
        if (!QSqlQuery().exec(table.query)) {
            qDebug() << "Ошибка: Не удалось создать таблицу " << table.name << ": " << QSqlQuery().lastError().text();
            return false;
        }
    }

    return true;
}

bool DatabaseManager::openDatabase() {
    if (!db.open()) {
        qDebug() << "Ошибка: Не удалось открыть базу данных." << db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::isUserExists(const QString &email, const QString &phone) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE email = :email OR phone = :phone");
    query.bindValue(":email", email);
    query.bindValue(":phone", phone);

    if (!query.exec()) {
        qDebug() << "Ошибка: не удалось проверить существование пользователя." << query.lastError().text();
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        return true;
    }

    return false;
}

QString DatabaseManager::hashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool DatabaseManager::registerUser(const QString &firstName, const QString &lastName, const QString &email, const QString &phone, const QString &password, const QString &country,
                                   const QString &currency, const QString &color_palette, const QString &font_size, const QString &background_music, const QString &two_factor_auth,
                                   const QString &photo, const QString drawing_key) {
    if (isUserExists(email, phone)) {
        qDebug() << "Ошибка: пользователь с таким email или номером телефона уже существует.";
        return false;
    }

    QSqlQuery query;

    QString hashedPassword = hashPassword(password);

    query.prepare("INSERT INTO users (first_name, last_name, email, phone, password, country, currency, color_palette, font_size, background_music, two_factor_auth, photo, drawing_key) "
                  "VALUES (:first_name, :last_name, :email, :phone, :password, :country, :currency, :color_palette, :font_size, :background_music, :two_factor_auth, :photo, :drawing_key)");

    query.bindValue(":first_name", firstName);
    query.bindValue(":last_name", lastName);
    query.bindValue(":email", email);
    query.bindValue(":phone", phone);
    query.bindValue(":password", hashedPassword);
    query.bindValue(":country", country);
    query.bindValue(":currency", currency);
    query.bindValue(":color_palette", color_palette);
    query.bindValue(":font_size", font_size);
    query.bindValue(":background_music", background_music);
    query.bindValue(":two_factor_auth", two_factor_auth);
    query.bindValue(":photo", photo);
    query.bindValue(":drawing_key", drawing_key);

    if (!query.exec()) {
        qDebug() << "Ошибка: не удалось зарегистрировать пользователя." << query.lastError().text();
        return false;
    }

    qint64 userId = query.lastInsertId().toLongLong();

    if (userId <= 0) {
        qDebug() << "Ошибка: не удалось получить идентификатор нового пользователя.";
        return false;
    }

    emit walletCountChanged(countWalletsByUser(userId), userId);
    emit updateExpenseData();
    return insertDefaultColors(userId);
}

bool DatabaseManager::insertDefaultColors(int userId) {
    QSqlQuery colorQuery;
    colorQuery.prepare(R"(
        INSERT INTO user_colors (user_id, color_white, color_background_main, color_background_block,
                                 color_background_header, color_background_menu, color_transparent_button,
                                 color_kard_kategory, color_button, color_dark, color_black)
        VALUES (:user_id, '#FFFFFF', '#F5F5F5', '#D9D9D9', '#808080', '#C0C0C0', '#808080', '#BEBEBE', '#A0A0A0', '#808080', '#000000');
    )");
    colorQuery.bindValue(":user_id", userId);

    if (!colorQuery.exec()) {
        qDebug() << "Ошибка: Не удалось вставить цвета по умолчанию в user_colors для пользователя" << userId << ":" << colorQuery.lastError().text();
        return false;
    }

    return true;
}

QString DatabaseManager::userId() const {
    return m_userId;
}

void DatabaseManager::setUserId(const QString &userId) {
    if (m_userId != userId) {
        m_userId = userId;
        emit userIdChanged();
    }
}

QString DatabaseManager::firstName() const {
    return m_firstName;
}

void DatabaseManager::setFirstName(const QString &firstName) {
    if (m_firstName != firstName) {
        m_firstName = firstName;
        emit firstNameChanged();
    }
}

QString DatabaseManager::lastName() const {
    return m_lastName;
}

void DatabaseManager::setLastName(const QString &lastName) {
    if (m_lastName != lastName) {
        m_lastName = lastName;
        emit lastNameChanged();
    }
}

QString DatabaseManager::email() const {
    return m_email;
}

void DatabaseManager::setEmail(const QString &email) {
    if (m_email != email) {
        m_email = email;
        emit emailChanged();
    }
}

QString DatabaseManager::phone() const {
    return m_phone;
}

void DatabaseManager::setPhone(const QString &phone) {
    if (m_phone != phone) {
        m_phone = phone;
        emit phoneChanged();
    }
}

QString DatabaseManager::password() const {
    return m_password;
}

void DatabaseManager::setPassword(const QString &password) {
    if (m_password != password) {
        m_password = password;
        emit passwordChanged();
    }
}

QString DatabaseManager::getCountryUser() const {
    return m_countryUser;
}

void DatabaseManager::setCountryUser(const QString &countryUser) {
    if (m_countryUser != countryUser) {
        m_countryUser = countryUser;
        QSqlQuery query;
        query.prepare("UPDATE users SET country = :country WHERE id = :id");
        query.bindValue(":country", countryUser);
        query.bindValue(":id", m_userId);
        if (!query.exec()) {
            qDebug() << "Ошибка обновления страны: " << query.lastError().text();
        }
        emit countryChanged();
    }
}

QString DatabaseManager::getCurrencyUser() const {
    return m_currencyUser;
}

void DatabaseManager::setCurrencyUser(const QString &currencyUser) {
    if (m_currencyUser != currencyUser) {
        m_currencyUser = currencyUser;
        QSqlQuery query;
        query.prepare("UPDATE users SET currency = :currency WHERE id = :id");
        query.bindValue(":currency", currencyUser);
        query.bindValue(":id", m_userId);
        if (!query.exec()) {
            qDebug() << "Ошибка обновления валюты: " << query.lastError().text();
        }
        emit currencyChanged();
    }
}

QString DatabaseManager::colorPalette() const {
    return m_colorPalette;
}

void DatabaseManager::setColorPalette(const QString &colorPalette) {
    if (m_colorPalette != colorPalette) {
        m_colorPalette = colorPalette;
        emit colorPaletteChanged();
    }
}

QString DatabaseManager::fontSize() const {
    return m_fontSize;
}

void DatabaseManager::setFontSize(const QString &fontSize) {
    if (m_fontSize != fontSize) {
        m_fontSize = fontSize;
        emit fontSizeChanged();
    }
}

QString DatabaseManager::backgroundMusic() const {
    return m_backgroundMusic;
}

void DatabaseManager::setBackgroundMusic(const QString &backgroundMusic) {
    if (m_backgroundMusic != backgroundMusic) {
        m_backgroundMusic = backgroundMusic;
        emit backgroundMusicChanged();
    }
}

QString DatabaseManager::getTwoFactorAuth() const {
    return m_twoFactorAuth;
}

void DatabaseManager::setTwoFactorAuth(const QString &twoFactorAuth) {
    if (m_twoFactorAuth != twoFactorAuth) {
        m_twoFactorAuth = twoFactorAuth;
        QSqlQuery query;
        query.prepare("UPDATE users SET two_factor_auth = :two_factor_auth WHERE id = :id");
        query.bindValue(":two_factor_auth", twoFactorAuth);
        query.bindValue(":id", m_userId);
        if (!query.exec()) {
            qDebug() << "Ошибка обновления 2fa: " << query.lastError().text();
        }
        emit twoFactorAuthChanged();
    }
}

QString DatabaseManager::photo() const {
    return m_photo;
}

void DatabaseManager::setPhoto(const QString &photo) {
    if (m_photo != photo) {
        m_photo = photo;
        emit photoChanged();
    }
}

QString DatabaseManager::drawing_key() const {
    return m_drawing_key;
}

void DatabaseManager::setDrawing_key(const QString &drawing_key) {
    if (m_drawing_key != drawing_key) {
        m_drawing_key = drawing_key;
        QSqlQuery query;
        query.prepare("UPDATE users SET drawing_key = :drawing_key WHERE id = :id");
        query.bindValue(":drawing_key", drawing_key);
        query.bindValue(":id", m_userId);
        if (!query.exec()) {
            qDebug() << "Ошибка при сохранении графического ключа:" << query.lastError().text();
        }
        emit drawingKeyChanged();
    }
}

bool DatabaseManager::checkDrawingKey(const QString &inputKey) {
    QSqlQuery query;
    query.prepare("SELECT drawing_key FROM users WHERE id = :id");
    query.bindValue(":id", m_userId);

    if (query.exec() && query.next()) {
        QString storedKey = query.value(0).toString();
        return storedKey == inputKey; // Сравниваем введенный ключ с сохраненным
    } else {
        qDebug() << "Ошибка при проверке графического ключа:" << query.lastError().text();
        return false;
    }
}

// Сохраняет текущую информацию о пользователе в базе данных
bool DatabaseManager::save() {
    QSqlQuery query;
    query.prepare("UPDATE users SET first_name = :first_name, last_name = :last_name, "
                  "email = :email, phone = :phone, country = :country, currency = :currency, photo = :photo, drawing_key = :drawing_key "
                  "WHERE id = :id");

    query.bindValue(":first_name", m_firstName);
    query.bindValue(":last_name", m_lastName);
    query.bindValue(":email", m_email);
    query.bindValue(":phone", m_phone);
    query.bindValue(":country", m_countryUser);
    query.bindValue(":currency", m_currencyUser);
    query.bindValue(":photo", m_photo);
    query.bindValue(":drawing_key", m_drawing_key);
    query.bindValue(":id", m_userId);

    if (!query.exec()) {
        qDebug() << "Ошибка: не удалось сохранить данные пользователя." << query.lastError().text();
        return false;
    }

    return true;
}

// Загружает информацию о пользователе из базы данных по логину
bool DatabaseManager::loadUserFromDB(const QString &login) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE email = :login OR phone = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
        qDebug() << "Ошибка: не удалось загрузить данные пользователя." << query.lastError().text();
        return false;
    }

    if (query.next()) {
        m_userId = query.value("id").toString();
        m_firstName = query.value("first_name").toString();
        m_lastName = query.value("last_name").toString();
        m_email = query.value("email").toString();
        m_phone = query.value("phone").toString();
        m_password = query.value("password").toString();
        m_countryUser = query.value("country").toString();
        m_currencyUser = query.value("currency").toString();
        m_colorPalette = query.value("color_palette").toString();
        m_fontSize = query.value("font_size").toString();
        m_backgroundMusic = query.value("background_music").toString();
        m_twoFactorAuth = query.value("two_factor_auth").toString();
        m_photo = query.value("photo").toString();
        m_drawing_key = query.value("drawing_key").toString();

        emit userIdChanged();
        emit firstNameChanged();
        emit lastNameChanged();
        emit emailChanged();
        emit phoneChanged();
        emit passwordChanged();
        emit countryChanged();
        emit currencyChanged();
        emit backgroundMusicChanged();
        emit twoFactorAuthChanged2();
        emit photoChanged();
        emit drawingKeyChanged();

        colorPalette();
        emit colorPaletteChanged();

        return true;
    } else {
        qDebug() << "Ошибка: пользователь не найден.";
        return false;
    }
}

bool DatabaseManager::createWallet(int userId, const QString& walletName) {
    if (userId <= 0 || walletName.isEmpty()) {
        qDebug() << "Ошибка: некорректные данные для создания кошелька.";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO wallets (user_id, wallet_name, balance) VALUES (:user_id, :wallet_name, :balance)");
    query.bindValue(":user_id", userId);
    query.bindValue(":wallet_name", walletName);
    query.bindValue(":balance", 0.0);

    if (!query.exec()) {
        qDebug() << "Ошибка при создании кошелька:" << query.lastError().text();
        return false;
    }

    emit walletCountChanged(countWalletsByUser(userId), userId);
    emit updateExpenseData();

    return true;
}

QVariantList DatabaseManager::getWalletsByUser(int userId) {
    QVariantList wallets;
    QSqlQuery query;
    query.prepare("SELECT id, wallet_name, balance FROM wallets WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap wallet;
            wallet["wallet_id"] = query.value(0).toInt();
            wallet["wallet_name"] = query.value(1).toString();
            wallet["balance"] = query.value(2).toDouble();
            wallets.append(wallet);
        }
    } else {
        qDebug() << "Error retrieving wallet list:" << query.lastError().text();
    }

    return wallets;
}

int DatabaseManager::countWalletsByUser(int userId) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM wallets WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if(userId != 0){
        if (query.exec()) {
            if (query.next()) {
                int walletCount = query.value(0).toInt();
                return walletCount;
            } else {
                qDebug() << "Ошибка: query.next() не вернул результат.";
            }
        } else {
            qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
        }
    }

    return 0;
}

QString DatabaseManager::getWalletNameById(int userId, int walletId) {
    QSqlQuery query;
    query.prepare("SELECT wallet_name FROM wallets WHERE user_id = :user_id AND id = :wallet_id");
    query.bindValue(":user_id", userId);
    query.bindValue(":wallet_id", walletId);

    if (query.exec() && query.next()) {
        return query.value("wallet_name").toString();
    } else {
        qDebug() << "Ошибка при получении имени кошелька:" << query.lastError().text();
        return QString();
    }
}

QVariantMap DatabaseManager::getMonthlyIncomeExpenses(QString walletName) {
    QVariantMap result;
    double totalIncome = 0.0;
    double totalExpenses = 0.0;

    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();
    QDateTime firstDayOfMonth(QDate(currentYear, currentMonth, 1), QTime(0, 0, 0));
    QDateTime endOfDay = QDateTime::currentDateTime();

    QString startDateStr = firstDayOfMonth.toString("dd.MM.yyyy HH:mm:ss");
    QString endDateStr = endOfDay.toString("dd.MM.yyyy HH:mm:ss");

    QSqlQuery query;
    query.prepare(R"(
        SELECT type, amount, date_time, status
        FROM transactions
        WHERE wallet = :wallet
        AND date_time >= :startDate
        AND date_time <= :endDate
    )");

    query.bindValue(":wallet", walletName);
    query.bindValue(":startDate", startDateStr);
    query.bindValue(":endDate", endDateStr);

    if (query.exec()) {
        while (query.next()) {
            QString status = query.value("status").toString();
            QString type = query.value("type").toString();
            double amount = query.value("amount").toDouble();
            QString dateTimeStr = query.value("date_time").toString();
            QDateTime transactionDate = QDateTime::fromString(dateTimeStr, "dd.MM.yyyy HH:mm:ss");

            if (transactionDate.date().year() == currentYear && transactionDate.date().month() == currentMonth) {
                if (type == "Доход" && status == "Выполнено") {
                    totalIncome += amount;
                } else if (type == "Расход" && status == "Выполнено") {
                    totalExpenses += amount;
                }
            }
        }
    } else {
        qWarning() << "Ошибка выполнения запроса для получения доходов и расходов:" << query.lastError().text();
    }

    result["income"] = totalIncome;
    result["expense"] = totalExpenses;

    return result;
}

bool DatabaseManager::updatePalette(const QString &palette) {
    QSqlQuery query;
    query.prepare("UPDATE users SET color_palette = :palette WHERE id = :id");
    query.bindValue(":palette", palette);
    query.bindValue(":id", m_userId);

    m_colorPalette = palette;

    if (!query.exec()) {
        qDebug() << "Ошибка обновления цветовой палитры:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::updateColorPalette(int userId, const QString& color_white, const QString& color_background_main,
                                         const QString& color_background_block, const QString& color_background_header,
                                         const QString& color_background_menu, const QString& color_transparent_button,
                                         const QString& color_kard_kategory, const QString& color_button,
                                         const QString& color_dark, const QString& color_black) {

    QString queryString = "UPDATE user_colors SET "
                          "color_white = :color_white, "
                          "color_background_main = :color_background_main, "
                          "color_background_block = :color_background_block, "
                          "color_background_header = :color_background_header, "
                          "color_background_menu = :color_background_menu, "
                          "color_transparent_button = :color_transparent_button, "
                          "color_kard_kategory = :color_kard_kategory, "
                          "color_button = :color_button, "
                          "color_dark = :color_dark, "
                          "color_black = :color_black "
                          "WHERE user_id = :user_id";

    QSqlQuery query;
    query.prepare(queryString);
    query.bindValue(":color_white", color_white);
    query.bindValue(":color_background_main", color_background_main);
    query.bindValue(":color_background_block", color_background_block);
    query.bindValue(":color_background_header", color_background_header);
    query.bindValue(":color_background_menu", color_background_menu);
    query.bindValue(":color_transparent_button", color_transparent_button);
    query.bindValue(":color_kard_kategory", color_kard_kategory);
    query.bindValue(":color_button", color_button);
    query.bindValue(":color_dark", color_dark);
    query.bindValue(":color_black", color_black);
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qDebug() << "Error updating colors: " << query.lastError().text();
        return false;
    }
    return true;
}

QVariantMap DatabaseManager::getUserColorPalette(int userId) {
    QVariantMap colors;
    QSqlQuery query;
    query.prepare("SELECT color_white, color_background_main, color_background_block, "
                  "color_background_header, color_background_menu, color_transparent_button, "
                  "color_kard_kategory, color_button, color_dark, color_black "
                  "FROM user_colors WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (query.exec() && query.next()) {
        colors["color_white"] = query.value(0).toString();
        colors["color_background_main"] = query.value(1).toString();
        colors["color_background_block"] = query.value(2).toString();
        colors["color_background_header"] = query.value(3).toString();
        colors["color_background_menu"] = query.value(4).toString();
        colors["color_transparent_button"] = query.value(5).toString();
        colors["color_kard_kategory"] = query.value(6).toString();
        colors["color_button"] = query.value(7).toString();
        colors["color_dark"] = query.value(8).toString();
        colors["color_black"] = query.value(9).toString();
    } else {
        qDebug() << "Ошибка при получении пользовательской цветовой палитры:" << query.lastError().text();
    }

    return colors;
}

QVariantMap DatabaseManager::getStandardPalette(const QString &palette) {
    QVariantMap colors;

    if (palette == "green") {
        colors["color_white"] = "#F7FFF8";
        colors["color_background_main"] = "#F3FFF4";
        colors["color_background_block"] = "#DDF4E6";
        colors["color_background_header"] = "#DAFFDD";
        colors["color_background_menu"] = "#A9DAC6";
        colors["color_transparent_button"] = "#91C3B7";
        colors["color_kard_kategory"] = "#80B4AE";
        colors["color_button"] = "#5F8F94";
        colors["color_dark"] = "#446B77";
        colors["color_black"] = "#2F4858";
    } else if (palette == "brown") {
        colors["color_white"] = "#FFFBE8";
        colors["color_background_main"] = "#FEF6CC";
        colors["color_background_block"] = "#DFBC8D";
        colors["color_background_header"] = "#CAA16A";
        colors["color_background_menu"] = "#CC8F53";
        colors["color_transparent_button"] = "#BF772F";
        colors["color_kard_kategory"] = "#A86828";
        colors["color_button"] = "#8E5A27";
        colors["color_dark"] = "#744D26";
        colors["color_black"] = "#4F3112";
    } else if (palette == "blue") {
        colors["color_white"] = "#F8F9FF";
        colors["color_background_main"] = "#EEF0FB";
        colors["color_background_block"] = "#D9DDF2";
        colors["color_background_header"] = "#B6BEEA";
        colors["color_background_menu"] = "#9BA4D7";
        colors["color_transparent_button"] = "#737EBD";
        colors["color_kard_kategory"] = "#7586E6";
        colors["color_button"] = "#6B7DE2";
        colors["color_dark"] = "#5361B0";
        colors["color_black"] = "#2F4858";
    } else {
        colors["color_white"] = "#FFFFFF";
        colors["color_background_main"] = "#F0F0F0";
        colors["color_background_block"] = "#D9D9D9";
        colors["color_background_header"] = "#B0B0B0";
        colors["color_background_menu"] = "#A0A0A0";
        colors["color_transparent_button"] = "#808080";
        colors["color_kard_kategory"] = "#BEBEBE";
        colors["color_button"] = "#A0A0A0";
        colors["color_dark"] = "#808080";
        colors["color_black"] = "#000000";
    }

    return colors;
}

QVariantList DatabaseManager::getCategoriesByUser(int userId) {
    QVariantList categories;
    QSqlQuery query;
    query.prepare("SELECT group_name, link, value, percentage FROM categories WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap categoryMap;
            categoryMap["group_name"] = query.value(0).toString();
            categoryMap["link"] = query.value(1).toString();
            categoryMap["value"] = query.value(2).toDouble();
            categoryMap["percentage"] = query.value(3).toDouble();
            categories.append(categoryMap);
        }
    } else {
        qDebug() << "Ошибка при получении категорий для пользователя:" << query.lastError().text();
    }

    return categories;
}

void DatabaseManager::addDataDB(int userId) {
    if (!db.isOpen()) {
        qDebug() << "База данных закрыта!";
        return;
    }

    QSqlQuery query(db);

    QList<QVariantMap> walletData = {
        {{"wallet_name", "0001"}, {"balance", 0.00}},
        {{"wallet_name", "0002"}, {"balance", 0.00}},
        {{"wallet_name", "0003"}, {"balance", 0.00}}
    };

    QList<QVariantMap> transactionData = {
        {{"date_time", "01.07.2024 06:00:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 10000.00},   {"previous_balance", 0.00},     {"new_balance", 10000.00}, {"wallet", "0001"}, {"category", "Пенсия"},                  {"target", ""},     {"source", "Чек"}, {"description", "1"}},
        {{"date_time", "01.07.2024 21:55:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 70.00},      {"previous_balance", 10000.00}, {"new_balance", 9930.00},  {"wallet", "0001"}, {"category", "Транспорт"},               {"target", ""},     {"source", "Чек"}, {"description", "2"}},
        {{"date_time", "03.07.2024 22:10:00"}, {"status", "Ошибка"},    {"type", "Расход"}, {"amount", 500.00},     {"previous_balance", 9930.00},  {"new_balance", 9930.00},  {"wallet", "0001"}, {"category", "Связь"},                   {"target", ""},     {"source", "Чек"}, {"description", "3"}},
        {{"date_time", "09.07.2024 14:30:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 11000.00},   {"previous_balance", 0.00},     {"new_balance", 11000.00}, {"wallet", "0002"}, {"category", "Подарки"},                 {"target", ""},     {"source", "Чек"}, {"description", "4"}},
        {{"date_time", "12.07.2024 18:15:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 800.00},     {"previous_balance", 11000.00}, {"new_balance", 10200.00}, {"wallet", "0002"}, {"category", "Услуги"},                  {"target", ""},     {"source", "Чек"}, {"description", "5"}},
        {{"date_time", "14.07.2024 10:59:00"}, {"status", "Отменено"},  {"type", "Доход"},  {"amount", 12000.00},   {"previous_balance", 0.00},     {"new_balance", 0.00},     {"wallet", "0003"}, {"category", "Онлайн-бизнес"},           {"target", ""},     {"source", "Чек"}, {"description", "6"}},
        {{"date_time", "20.07.2024 11:59:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 12000.00},   {"previous_balance", 0.00},     {"new_balance", 12000.00}, {"wallet", "0003"}, {"category", "Онлайн-бизнес"},           {"target", ""},     {"source", "Чек"}, {"description", "7"}},
        {{"date_time", "22.07.2024 19:25:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 300.00},     {"previous_balance", 12000.00}, {"new_balance", 11700.00}, {"wallet", "0003"}, {"category", "Образование"},             {"target", ""},     {"source", "Чек"}, {"description", "8"}},
        {{"date_time", "24.07.2024 20:27:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 630.00},     {"previous_balance", 9930.00},  {"new_balance", 9300.00},  {"wallet", "0001"}, {"category", "Культура"},                {"target", ""},     {"source", "Чек"}, {"description", "9"}},
        {{"date_time", "29.07.2024 09:10:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 3000.00},    {"previous_balance", 9300.00},  {"new_balance", 12300.00}, {"wallet", "0001"}, {"category", "Дотации и субсидии"},      {"target", ""},     {"source", "Чек"}, {"description", "10"}},

        {{"date_time", "10.08.2024 14:45:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 1500.00},    {"previous_balance", 12300.00}, {"new_balance", 10800.00}, {"wallet", "0001"}, {"category", "Транспорт"},               {"target", ""},     {"source", "Чек"}, {"description", "11"}},
        {{"date_time", "11.08.2024 11:30:00"}, {"status", "Ошибка"},    {"type", "Доход"},  {"amount", 2000.00},    {"previous_balance", 10200.00}, {"new_balance", 10200.00}, {"wallet", "0002"}, {"category", "Проценты по вкладам"},     {"target", ""},     {"source", "Чек"}, {"description", "12"}},
        {{"date_time", "13.08.2024 17:50:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 420.00},     {"previous_balance", 10800.00}, {"new_balance", 10380.00}, {"wallet", "0001"}, {"category", "Косметика и гигиена"},     {"target", ""},     {"source", "Чек"}, {"description", "13"}},
        {{"date_time", "15.08.2024 20:10:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 7000.00},    {"previous_balance", 10380.00}, {"new_balance", 17380.00}, {"wallet", "0001"}, {"category", "Фриланс"},                 {"target", ""},     {"source", "Чек"}, {"description", "14"}},
        {{"date_time", "17.08.2024 15:05:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 800.00},     {"previous_balance", 10200.00}, {"new_balance", 9400.00},  {"wallet", "0002"}, {"category", "Развлечения и досуг"},     {"target", ""},     {"source", "Чек"}, {"description", "15"}},
        {{"date_time", "18.08.2024 09:25:00"}, {"status", "Отменено"},  {"type", "Доход"},  {"amount", 4500.00},    {"previous_balance", 11700.00}, {"new_balance", 11700.00}, {"wallet", "0003"}, {"category", "Стипендия"},               {"target", ""},     {"source", "Чек"}, {"description", "16"}},
        {{"date_time", "19.08.2024 08:40:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 3200.00},    {"previous_balance", 9400.00},  {"new_balance", 12600.00}, {"wallet", "0002"}, {"category", "Инвестиции"},              {"target", ""},     {"source", "Чек"}, {"description", "17"}},
        {{"date_time", "21.08.2024 18:35:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 750.00},     {"previous_balance", 17380.00}, {"new_balance", 16630.00}, {"wallet", "0001"}, {"category", "Кредит"},                  {"target", ""},     {"source", "Чек"}, {"description", "18"}},
        {{"date_time", "23.08.2024 19:20:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 9500.00},    {"previous_balance", 16630.00}, {"new_balance", 26130.00}, {"wallet", "0001"}, {"category", "Аренда"},                  {"target", ""},     {"source", "Чек"}, {"description", "19"}},
        {{"date_time", "25.08.2024 10:55:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 670.00},     {"previous_balance", 26130.00}, {"new_balance", 25460.00}, {"wallet", "0001"}, {"category", "Одежда и обувь"},          {"target", ""},     {"source", "Чек"}, {"description", "20"}},

        {{"date_time", "01.09.2024 06:30:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 12000.00},   {"previous_balance", 25460.00}, {"new_balance", 37460.00}, {"wallet", "0001"}, {"category", "Дивиденды"},               {"target", ""},     {"source", "Чек"}, {"description", "21"}},
        {{"date_time", "01.09.2024 08:15:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 360.00},     {"previous_balance", 37460.00}, {"new_balance", 37100.00}, {"wallet", "0001"}, {"category", "Спорт и фитнес"},          {"target", ""},     {"source", "Чек"}, {"description", "22"}},
        {{"date_time", "03.09.2024 21:10:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 8000.00},    {"previous_balance", 11700.00}, {"new_balance", 19700.00}, {"wallet", "0003"}, {"category", "Онлайн-бизнес"},           {"target", ""},     {"source", "Чек"}, {"description", "23"}},
        {{"date_time", "05.09.2024 10:50:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 220.00},     {"previous_balance", 12600.00}, {"new_balance", 12380.00}, {"wallet", "0002"}, {"category", "Книги"},                   {"target", ""},     {"source", "Чек"}, {"description", "24"}},
        {{"date_time", "07.09.2024 14:00:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 400.00},     {"previous_balance", 37100.00}, {"new_balance", 36700.00}, {"wallet", "0001"}, {"category", "Подарки и благотворит."},  {"target", ""},     {"source", "Чек"}, {"description", "25"}},
        {{"date_time", "08.09.2024 20:30:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 17000.00},   {"previous_balance", 36700.00}, {"new_balance", 53700.00}, {"wallet", "0001"}, {"category", "Авторские гонорары"},      {"target", ""},     {"source", "Чек"}, {"description", "26"}},
        {{"date_time", "10.09.2024 11:30:00"}, {"status", "Ошибка"},    {"type", "Доход"},  {"amount", 5000.00},    {"previous_balance", 53700.00}, {"new_balance", 53700.00}, {"wallet", "0001"}, {"category", "Заработная плата"},        {"target", ""},     {"source", "Чек"}, {"description", "27"}},
        {{"date_time", "13.09.2024 16:45:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 700.00},     {"previous_balance", 19700.00}, {"new_balance", 19000.00}, {"wallet", "0003"}, {"category", "Продукты питания"},        {"target", ""},     {"source", "Чек"}, {"description", "28"}},
        {{"date_time", "15.09.2024 09:10:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 1500.00},    {"previous_balance", 19000.00}, {"new_balance", 17500.00}, {"wallet", "0003"}, {"category", "Жилище и ком. услуги"},    {"target", ""},     {"source", "Чек"}, {"description", "29"}},
        {{"date_time", "20.09.2024 14:20:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 2500.00},    {"previous_balance", 12380.00}, {"new_balance", 14880.00}, {"wallet", "0002"}, {"category", "Бонусы и премии"},         {"target", ""},     {"source", "Чек"}, {"description", "30"}},

        {{"date_time", "05.10.2024 19:00:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 1200.00},    {"previous_balance", 14880.00}, {"new_balance", 13680.00}, {"wallet", "0002"}, {"category", "Развлечения и досуг"},     {"target", ""},     {"source", "Чек"}, {"description", "31"}},
        {{"date_time", "15.10.2024 08:15:00"}, {"status", "Отменено"},  {"type", "Расход"}, {"amount", 300.00},     {"previous_balance", 53700.00}, {"new_balance", 53700.00}, {"wallet", "0001"}, {"category", "Товары для дома"},         {"target", ""},     {"source", "Чек"}, {"description", "32"}},
        {{"date_time", "24.10.2024 13:50:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 4500.00},    {"previous_balance", 17500.00}, {"new_balance", 22000.00}, {"wallet", "0003"}, {"category", "Инвестиции"},              {"target", ""},     {"source", "Чек"}, {"description", "33"}},
        {{"date_time", "24.10.2024 18:45:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 2200.00},    {"previous_balance", 22000.00}, {"new_balance", 19800.00}, {"wallet", "0003"}, {"category", "Книги"},                   {"target", ""},     {"source", "Чек"}, {"description", "34"}},
        {{"date_time", "25.10.2024 10:00:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 3000.00},    {"previous_balance", 13680.00}, {"new_balance", 16680.00}, {"wallet", "0002"}, {"category", "Фриланс"},                 {"target", ""},     {"source", "Чек"}, {"description", "35"}},
        {{"date_time", "27.10.2024 12:30:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 500.00},     {"previous_balance", 16680.00}, {"new_balance", 16180.00}, {"wallet", "0002"}, {"category", "Косметика и гигиена"},     {"target", ""},     {"source", "Чек"}, {"description", "36"}},
        {{"date_time", "27.10.2024 16:30:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 7000.00},    {"previous_balance", 53700.00}, {"new_balance", 60700.00}, {"wallet", "0001"}, {"category", "Дивиденды"},               {"target", ""},     {"source", "Чек"}, {"description", "37"}},
        {{"date_time", "28.10.2024 09:50:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 1800.00},    {"previous_balance", 60700.00}, {"new_balance", 58900.00}, {"wallet", "0001"}, {"category", "Здоровье и медицина"},     {"target", ""},     {"source", "Чек"}, {"description", "38"}},
        {{"date_time", "29.10.2024 08:00:00"}, {"status", "Ошибка"},    {"type", "Доход"},  {"amount", 4000.00},    {"previous_balance", 16180.00}, {"new_balance", 16180.00}, {"wallet", "0002"}, {"category", "Аренда"},                  {"target", ""},     {"source", "Чек"}, {"description", "39"}},
        {{"date_time", "30.10.2024 18:15:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 600.00},     {"previous_balance", 16180.00}, {"new_balance", 15580.00}, {"wallet", "0002"}, {"category", "Спорт и фитнес"},          {"target", ""},     {"source", "Чек"}, {"description", "40"}},

        {{"date_time", "05.11.2024 13:25:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 2000.00},    {"previous_balance", 19800.00}, {"new_balance", 21800.00}, {"wallet", "0003"}, {"category", "Дотации и субсидии"},      {"target", ""},     {"source", "Чек"}, {"description", "41"}},
        {{"date_time", "08.11.2024 14:50:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 1200.00},    {"previous_balance", 21800.00}, {"new_balance", 20600.00}, {"wallet", "0003"}, {"category", "Хобби"},                   {"target", ""},     {"source", "Чек"}, {"description", "42"}},
        {{"date_time", "10.11.2024 17:30:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 5000.00},    {"previous_balance", 58900.00}, {"new_balance", 63900.00}, {"wallet", "0001"}, {"category", "Проценты по вкладам"},     {"target", ""},     {"source", "Чек"}, {"description", "43"}},
        {{"date_time", "12.11.2024 11:10:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 300.00},     {"previous_balance", 63900.00}, {"new_balance", 63600.00}, {"wallet", "0001"}, {"category", "Домашние животные"},       {"target", ""},     {"source", "Чек"}, {"description", "44"}},
        {{"date_time", "15.11.2024 16:45:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 8000.00},    {"previous_balance", 15580.00}, {"new_balance", 23580.00}, {"wallet", "0002"}, {"category", "Прибыль от партнерств"},   {"target", ""},     {"source", "Чек"}, {"description", "45"}},
        {{"date_time", "17.11.2024 18:00:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 1300.00},    {"previous_balance", 23580.00}, {"new_balance", 22280.00}, {"wallet", "0002"}, {"category", "Кредит"},                  {"target", ""},     {"source", "Чек"}, {"description", "46"}},
        {{"date_time", "19.11.2024 10:20:00"}, {"status", "Выполнено"}, {"type", "Доход"},  {"amount", 1500.00},    {"previous_balance", 20600.00}, {"new_balance", 22100.00}, {"wallet", "0003"}, {"category", "Стипендия"},               {"target", ""},     {"source", "Чек"}, {"description", "47"}},
        {{"date_time", "20.11.2024 12:15:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 500.00},     {"previous_balance", 22100.00}, {"new_balance", 21600.00}, {"wallet", "0003"}, {"category", "Путешествия"},             {"target", ""},     {"source", "Чек"}, {"description", "48"}},
        {{"date_time", "21.11.2024 09:30:00"}, {"status", "Ошибка"},    {"type", "Доход"},  {"amount", 3000.00},    {"previous_balance", 63600.00}, {"new_balance", 63600.00}, {"wallet", "0001"}, {"category", "Подарки"},                 {"target", ""},     {"source", "Чек"}, {"description", "49"}},
        {{"date_time", "22.11.2024 08:30:00"}, {"status", "Выполнено"}, {"type", "Расход"}, {"amount", 1000.00},    {"previous_balance", 63600.00}, {"new_balance", 62600.00}, {"wallet", "0001"}, {"category", "Образование"},             {"target", ""},     {"source", "Чек"}, {"description", "50"}}
    };

    QVector<QVariantMap> plansData = {
        {{"type", "Неделя"},    {"name", "Обновление гардероба"},       {"amount", 5000.00},    {"description", "Приобрести новые вещи для повседневной носки"}},
        {{"type", "Месяц"},     {"name", "Гаджет для работы"},          {"amount", 20000.00},   {"description", "Новый ноутбук для работы и учебы"}},
        {{"type", "Год"},       {"name", "Семейный автомобиль"},        {"amount", 800000.00},  {"description", "Автомобиль для поездок всей семьей"}},
        {{"type", "Неделя"},    {"name", "Кухонные принадлежности"},    {"amount", 1500.00},    {"description", "Набор новых кастрюль и сковородок"}},
        {{"type", "Месяц"},     {"name", "Мебель для гостиной"},        {"amount", 35000.00},   {"description", "Диван и кресла для уютной гостиной"}},
        {{"type", "Год"},       {"name", "Техника для дома"},           {"amount", 100000.00},  {"description", "Новая стиральная машина и холодильник"}},
        {{"type", "Неделя"},    {"name", "Продукты питания"},           {"amount", 2500.00},    {"description", "Закупка продуктов на неделю"}},
        {{"type", "Месяц"},     {"name", "Оплата коммунальных услуг"},  {"amount", 12000.00},   {"description", "Оплата электричества, воды и газа"}},
        {{"type", "Год"},       {"name", "Отпуск на море"},             {"amount", 150000.00},  {"description", "Планируется поездка к морю с семьей"}},
        {{"type", "Неделя"},    {"name", "Спортивный инвентарь"},       {"amount", 3000.00},    {"description", "Купить новую форму и аксессуары для фитнеса"}},
        {{"type", "Месяц"},     {"name", "Ремонт бытовой техники"},     {"amount", 7000.00},    {"description", "Починка пылесоса и микроволновки"}},
        {{"type", "Год"},       {"name", "Ремонт квартиры"},            {"amount", 500000.00},  {"description", "Ремонт кухни и ванной комнаты"}},
        {{"type", "Неделя"},    {"name", "Книги и журналы"},            {"amount", 2000.00},    {"description", "Покупка новых книг для чтения"}},
        {{"type", "Месяц"},     {"name", "Образование и курсы"},        {"amount", 15000.00},   {"description", "Оплата за курсы повышения квалификации"}},
        {{"type", "Год"},       {"name", "Сбережения на будущее"},      {"amount", 300000.00},  {"description", "Формирование финансовой подушки безопасности"}},
        {{"type", "Неделя"},    {"name", "Развлечения"},                {"amount", 500.00},     {"description", "Посещение кинотеатра или кафе"}},
        {{"type", "Месяц"},     {"name", "Ремонт автомобиля"},          {"amount", 10000.00},   {"description", "Плановый техосмотр и замена масла"}},
        {{"type", "Год"},       {"name", "Образование детей"},          {"amount", 200000.00},  {"description", "Оплата обучения в школе или университете"}}
    };

    QVector<QVariantMap> taregtsData = {
        {{"date_start", "01.07.2023"}, {"target_name", "Обновление гардероба"},       {"target_amount", 5000.00},    {"saved_amount", "0,00"}, {"category", "Одежда и обувь"},        {"description", "Приобрести новые вещи для повседневной носки"},    {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "23.08.2023"}, {"target_name", "Гаджет для работы"},          {"target_amount", 20000.00},   {"saved_amount", "0,00"}, {"category", "Гаджеты"},               {"description", "Новый ноутбук для работы и учебы"},                {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "14.09.2023"}, {"target_name", "Семейный автомобиль"},        {"target_amount", 800000.00},  {"saved_amount", "0,00"}, {"category", "Транспорт"},             {"description", "Автомобиль для поездок всей семьей"},              {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "03.11.2023"}, {"target_name", "Кухонные принадлежности"},    {"target_amount", 1500.00},    {"saved_amount", "0,00"}, {"category", "Товары для дома"},       {"description", "Набор новых кастрюль и сковородок"},               {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "30.12.2023"}, {"target_name", "Мебель для гостиной"},        {"target_amount", 35000.00},   {"saved_amount", "0,00"}, {"category", "Товары для дома"},       {"description", "Диван и кресла для уютной гостиной"},              {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "01.01.2024"}, {"target_name", "Техника для дома"},           {"target_amount", 100000.00},  {"saved_amount", "0,00"}, {"category", "Товары для дома"},       {"description", "Новая стиральная машина и холодильник"},           {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "30.03.2024"}, {"target_name", "Продукты питания"},           {"target_amount", 2500.00},    {"saved_amount", "0,00"}, {"category", "Продукты питания"},      {"description", "Закупка продуктов на неделю"},                     {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "09.06.2024"}, {"target_name", "Оплата коммунальных услуг"},  {"target_amount", 12000.00},   {"saved_amount", "0,00"}, {"category", "Жилище и ком. услуги"},  {"description", "Оплата электричества, воды и газа"},               {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "06.07.2024"}, {"target_name", "Отпуск на море"},             {"target_amount", 150000.00},  {"saved_amount", "0,00"}, {"category", "Путешествия"},           {"description", "Планируется поездка к морю с семьей"},             {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "01.08.2024"}, {"target_name", "Спортивный инвентарь"},       {"target_amount", 3000.00},    {"saved_amount", "0,00"}, {"category", "Спорт и фитнес"},        {"description", "Купить новую форму и аксессуары для фитнеса"},     {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "07.09.2024"}, {"target_name", "Ремонт бытовой техники"},     {"target_amount", 7000.00},    {"saved_amount", "0,00"}, {"category", "Услуги"},                {"description", "Починка пылесоса и микроволновки"},                {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "21.10.2024"}, {"target_name", "Ремонт квартиры"},            {"target_amount", 500000.00},  {"saved_amount", "0,00"}, {"category", "Услуги"},                {"description", "Ремонт кухни и ванной комнаты"},                   {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "30.11.2024"}, {"target_name", "Книги и журналы"},            {"target_amount", 2000.00},    {"saved_amount", "0,00"}, {"category", "Книги"},                 {"description", "Покупка новых книг для чтения"},                   {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "08.12.2024"}, {"target_name", "Образование и курсы"},        {"target_amount", 15000.00},   {"saved_amount", "0,00"}, {"category", "Образование"},           {"description", "Оплата за курсы повышения квалификации"},          {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "17.12.2024"}, {"target_name", "Сбережения на будущее"},      {"target_amount", 300000.00},  {"saved_amount", "0,00"}, {"category", "Сборы"},                 {"description", "Формирование финансовой подушки безопасности"},    {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "26.01.2025"}, {"target_name", "Развлечения"},                {"target_amount", 500.00},     {"saved_amount", "0,00"}, {"category", "Развлечения и досуг"},   {"description", "Посещение кинотеатра или кафе"},                   {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "28.01.2025"}, {"target_name", "Ремонт автомобиля"},          {"target_amount", 10000.00},   {"saved_amount", "0,00"}, {"category", "Автомобиль"},            {"description", "Плановый техосмотр и замена масла"},               {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}},
        {{"date_start", "03.02.2025"}, {"target_name", "Образование детей"},          {"target_amount", 200000.00},  {"saved_amount", "0,00"}, {"category", "Образование"},           {"description", "Оплата обучения в школе или университете"},        {"source_1", "Чек"}, {"source_2", "Чек"}, {"source_3", "Чек"}}
    };

    db.transaction();

    for (const QVariantMap &wallet : walletData) {
        query.prepare("INSERT INTO wallets (user_id, wallet_name, balance) VALUES (:user_id, :wallet_name, :balance)");
        query.bindValue(":user_id", userId);
        query.bindValue(":wallet_name", wallet["wallet_name"]);
        query.bindValue(":balance", wallet["balance"]);

        if (!query.exec()) {
            qDebug() << "Ошибка добавления данных в wallets:" << query.lastError().text();
        }
    }

    for (const QVariantMap &transaction : transactionData) {
        query.prepare(
            "INSERT INTO transactions (user_id, date_time, status, type, amount, previous_balance, new_balance, wallet, category, source, description) "
            "VALUES (:user_id, :date_time, :status, :type, :amount, :previous_balance, :new_balance, :wallet, :category, :source, :description)");
        query.bindValue(":user_id", userId);
        query.bindValue(":date_time", transaction["date_time"]);
        query.bindValue(":status", transaction["status"]);
        query.bindValue(":type", transaction["type"]);
        query.bindValue(":amount", transaction["amount"]);
        query.bindValue(":previous_balance", transaction["previous_balance"]);
        query.bindValue(":new_balance", transaction["new_balance"]);
        query.bindValue(":wallet", transaction["wallet"]);
        query.bindValue(":category", transaction["category"]);
        query.bindValue(":source", transaction["source"]);
        query.bindValue(":description", transaction["description"]);

        if (!query.exec()) {
            qDebug() << "Ошибка добавления данных в transactions:" << query.lastError().text();
        }
    }

    for (const QVariantMap &plans : plansData) {
        query.prepare("INSERT INTO plans (user_id, type, name, amount, description) VALUES (:user_id, :type, :name, :amount, :description);");
        query.bindValue(":user_id", userId);
        query.bindValue(":type", plans["type"]);
        query.bindValue(":name", plans["name"]);
        query.bindValue(":amount", plans["amount"]);
        query.bindValue(":description", plans["description"]);
        if (!query.exec()) {
            qWarning() << "Ошибка добавления данных в plans:" << query.lastError().text();
        }
    }

    for (const QVariantMap &targets : taregtsData) {
        query.prepare("INSERT INTO targets (user_id, date_start, target_name, target_amount, saved_amount, category, description, source_1, source_2, source_3) VALUES (:user_id, :date_start, :target_name, :target_amount, :saved_amount, :category, :description, :source_1, :source_2, :source_3);");
        query.bindValue(":user_id", userId);
        query.bindValue(":date_start", targets["date_start"]);
        query.bindValue(":target_name", targets["target_name"]);
        query.bindValue(":target_amount", targets["target_amount"]);
        query.bindValue(":saved_amount", targets["saved_amount"]);
        query.bindValue(":category", targets["category"]);
        query.bindValue(":description", targets["description"]);
        query.bindValue(":source_1", targets["source_1"]);
        query.bindValue(":source_2", targets["source_2"]);
        query.bindValue(":source_3", targets["source_3"]);
        if (!query.exec()) {
            qWarning() << "Ошибка добавления данных в targets:" << query.lastError().text();
        }
    }

    if (!db.commit()) {
        qDebug() << "Не удалось завершить транзакцию!" << db.lastError().text();
    } else {
        qDebug() << "Данные были успешно добавлены";
    }
}

bool DatabaseManager::deleteUserData(int userId) {
    if (userId <= 0) {
        qDebug() << "Ошибка: некорректный идентификатор пользователя.";
        return false;
    }

    if (!db.transaction()) {
        qDebug() << "Ошибка: не удалось начать транзакцию." << db.lastError().text();
        return false;
    }

    // Удаляем транзакции пользователя
    QSqlQuery deleteTransactionsQuery;
    deleteTransactionsQuery.prepare("DELETE FROM transactions WHERE user_id = :user_id");
    deleteTransactionsQuery.bindValue(":user_id", userId);
    if (!deleteTransactionsQuery.exec()) {
        qDebug() << "Ошибка при удалении транзакций:" << deleteTransactionsQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Удаляем планы пользователя
    QSqlQuery deletePlansQuery;
    deletePlansQuery.prepare("DELETE FROM plans WHERE user_id = :user_id");
    deletePlansQuery.bindValue(":user_id", userId);
    if (!deletePlansQuery.exec()) {
        qDebug() << "Ошибка при удалении планов:" << deletePlansQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Удаляем цели пользователя
    QSqlQuery deleteTargetsQuery;
    deleteTargetsQuery.prepare("DELETE FROM targets WHERE user_id = :user_id");
    deleteTargetsQuery.bindValue(":user_id", userId);
    if (!deleteTargetsQuery.exec()) {
        qDebug() << "Ошибка при удалении целей:" << deleteTargetsQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Удаляем историю целей
    QSqlQuery deleteTargetHistoryQuery;
    deleteTargetHistoryQuery.prepare("DELETE FROM target_history WHERE target_id IN (SELECT id FROM targets WHERE user_id = :user_id)");
    deleteTargetHistoryQuery.bindValue(":user_id", userId);
    if (!deleteTargetHistoryQuery.exec()) {
        qDebug() << "Ошибка при удалении истории целей:" << deleteTargetHistoryQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Удаляем кошельки пользователя
    QSqlQuery deleteWalletsQuery;
    deleteWalletsQuery.prepare("DELETE FROM wallets WHERE user_id = :user_id");
    deleteWalletsQuery.bindValue(":user_id", userId);
    if (!deleteWalletsQuery.exec()) {
        qDebug() << "Ошибка при удалении кошельков:" << deleteWalletsQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Удаляем пользователя
    QSqlQuery deleteUserQuery;
    deleteUserQuery.prepare("DELETE FROM users WHERE id = :user_id");
    deleteUserQuery.bindValue(":user_id", userId);
    if (!deleteUserQuery.exec()) {
        qDebug() << "Ошибка при удалении пользователя:" << deleteUserQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Завершаем транзакцию
    if (!db.commit()) {
        qDebug() << "Ошибка: не удалось завершить транзакцию." << db.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::authenticateUserWithIp(const QString &login, const QString &password, const QString &ipAddress) {
    // Проверка на заблокированный IP-адрес
    if (isIpBanned(ipAddress)) {
        qDebug() << "Доступ запрещен. IP-адрес заблокирован.";
        return false;
    }

    QSqlQuery query;
    QString hashedPassword = hashPassword(password);

    query.prepare("SELECT id FROM users WHERE (email = :login OR phone = :login) AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", hashedPassword);

    if (!query.exec()) {
        qDebug() << "Ошибка: не удалось выполнить запрос на аутентификацию." << query.lastError().text();
        incrementFailedLoginAttempts(ipAddress);
        return false;
    }

    if (query.next()) {
        resetFailedLoginAttempts(ipAddress); // Сбрасываем попытки при успешной аутентификации
        int userId = query.value(0).toInt();
        emit walletCountChanged(countWalletsByUser(userId), userId);
        emit updateExpenseData();
        return true;
    } else {
        incrementFailedLoginAttempts(ipAddress); // Увеличиваем счетчик при неудаче
        return false;
    }
}

bool DatabaseManager::isIpBanned(const QString &ipAddress) {
    QSqlQuery query;
    query.prepare("SELECT ban_time, ban_duration FROM ip_bans WHERE ip_address = :ip_address");
    query.bindValue(":ip_address", ipAddress);

    if (query.exec() && query.next()) {
        QDateTime banTime = query.value(0).toDateTime();
        int banDuration = query.value(1).toInt();

        // Проверяем, истекло ли время блокировки
        if (banTime.addSecs(banDuration * 60) > QDateTime::currentDateTime()) {
            return true; // IP все еще заблокирован
        } else {
            // Удаляем запись о блокировке, если время истекло
            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM ip_bans WHERE ip_address = :ip_address");
            deleteQuery.bindValue(":ip_address", ipAddress);
            deleteQuery.exec();
        }
    }

    return false; // IP не заблокирован
}

void DatabaseManager::incrementFailedLoginAttempts(const QString &ipAddress) {
    QSqlQuery query;

    // Проверяем, есть ли запись для данного IP
    query.prepare("SELECT attempt_count FROM failed_logins WHERE ip_address = :ip_address");
    query.bindValue(":ip_address", ipAddress);

    int count = 0;
    if (query.exec() && query.next()) {
        count = query.value(0).toInt();
    }

    if (count >= 2) { // Блокируем после третьей попытки
        banIpAddress(ipAddress); // Блокируем IP-адрес
        qDebug() << "Доступ запрещен. IP-адрес заблокирован после 3 неудачных попыток.";
    } else {
        // Если запись существует, обновляем попытки
        if (count > 0) {
            query.prepare("UPDATE failed_logins SET attempt_count = :new_count WHERE ip_address = :ip_address");
        } else {
            // Если записи нет, вставляем новую
            query.prepare("INSERT INTO failed_logins (ip_address, attempt_count) VALUES (:ip_address, :new_count)");
        }
        query.bindValue(":ip_address", ipAddress);
        query.bindValue(":new_count", count + 1);

        if (!query.exec()) {
            qDebug() << "Ошибка при обновлении попыток входа." << query.lastError().text();
        }
    }
}

void DatabaseManager::resetFailedLoginAttempts(const QString &ipAddress) {
    QSqlQuery query;
    query.prepare("DELETE FROM failed_logins WHERE ip_address = :ip_address");
    query.bindValue(":ip_address", ipAddress);
    if (!query.exec()) {
        qDebug() << "Ошибка при сбросе попыток входа." << query.lastError().text();
    }
}

void DatabaseManager::banIpAddress(const QString &ipAddress) {
    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO ip_bans (ip_address, ban_time, ban_duration) VALUES (:ip_address, :ban_time, :ban_duration)");
    query.bindValue(":ip_address", ipAddress);
    query.bindValue(":ban_time", QDateTime::currentDateTime());
    query.bindValue(":ban_duration", 3); // 3 минут блокировки

    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении IP-адреса в бан-лист." << query.lastError().text();
    } else {
        qDebug() << "IP-адрес добавлен в бан-лист: " << ipAddress;
    }

    // Удаляем записи о неудачных попытках после блокировки
    resetFailedLoginAttempts(ipAddress);
}

bool DatabaseManager::changeUserPassword(const QString &oldPassword, const QString &newPassword) {
    QString currentPasswordHash = password();
    QString hashedOldPassword = hashPassword(oldPassword);

    if (hashedOldPassword != currentPasswordHash) {
        qDebug() << "Ошибка: старый пароль неверный.";
        return false;
    }

    QString hashedNewPassword = hashPassword(newPassword);

    QSqlQuery query;
    query.prepare("UPDATE users SET password = :new_password WHERE id = :id");
    query.bindValue(":new_password", hashedNewPassword);
    query.bindValue(":id", m_userId);

    if (!query.exec()) {
        qDebug() << "Ошибка при обновлении пароля:" << query.lastError().text();
        return false;
    }

    return true;
}

TransactionModel* DatabaseManager::getTransactionModel() const {
    return m_transactionModel;
}
