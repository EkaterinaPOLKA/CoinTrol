#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariantMap>
#include <QVariantList>

// Предварительное объявление класса TransactionModel
class TransactionModel;

class DatabaseManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY firstNameChanged)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY lastNameChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString phone READ phone WRITE setPhone NOTIFY phoneChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString country READ getCountryUser WRITE setCountryUser NOTIFY countryChanged)
    Q_PROPERTY(QString currency READ getCurrencyUser WRITE setCurrencyUser NOTIFY currencyChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString twoFactorAuth READ getTwoFactorAuth WRITE setTwoFactorAuth NOTIFY twoFactorAuthChanged)
    Q_PROPERTY(QString drawing_key READ drawing_key WRITE setDrawing_key NOTIFY drawingKeyChanged FINAL)

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    QSqlDatabase& database() { return db; }

    // Проверяет, существует ли пользователь с указанным электронным адресом или номером телефона
    Q_INVOKABLE bool isUserExists(const QString &email, const QString &phone);

    // Хеширует заданный пароль
    Q_INVOKABLE QString hashPassword(const QString &password);

    // Регистрирует нового пользователя с указанными данными
    Q_INVOKABLE bool registerUser(const QString &firstName, const QString &lastName, const QString &email, const QString &phone, const QString &password, const QString &country, const QString &currency, const QString &color_palette, const QString &font_size, const QString &background_music, const QString &two_factor_auth, const QString &photo, const QString drawing_key);

    // Добавляет значения по умолчанию в таблицу палитры
    Q_INVOKABLE bool insertDefaultColors(int userId);

    Q_INVOKABLE QString userId() const;
    Q_INVOKABLE void setUserId(const QString &userId);

    Q_INVOKABLE QString firstName() const;
    Q_INVOKABLE void setFirstName(const QString &firstName);

    Q_INVOKABLE QString lastName() const;
    Q_INVOKABLE void setLastName(const QString &lastName);

    Q_INVOKABLE QString email() const;
    Q_INVOKABLE void setEmail(const QString &email);

    Q_INVOKABLE QString phone() const;
    Q_INVOKABLE void setPhone(const QString &phone);

    Q_INVOKABLE QString password() const;
    Q_INVOKABLE void setPassword(const QString &password);

    Q_INVOKABLE QString getCountryUser() const;
    Q_INVOKABLE void setCountryUser(const QString &countryUser);

    Q_INVOKABLE QString getCurrencyUser() const;
    Q_INVOKABLE void setCurrencyUser(const QString &currencyUser);

    Q_INVOKABLE QString colorPalette() const;
    Q_INVOKABLE void setColorPalette(const QString &colorPalette);

    Q_INVOKABLE QString fontSize() const;
    Q_INVOKABLE void setFontSize(const QString &fontSize);

    Q_INVOKABLE QString backgroundMusic() const;
    Q_INVOKABLE void setBackgroundMusic(const QString &backgroundMusic);

    Q_INVOKABLE QString getTwoFactorAuth() const;
    Q_INVOKABLE void setTwoFactorAuth(const QString &twoFactorAuth);

    Q_INVOKABLE QString photo() const;
    Q_INVOKABLE void setPhoto(const QString &photo);

    Q_INVOKABLE QString drawing_key() const;
    Q_INVOKABLE void setDrawing_key(const QString &drawing_key);
    Q_INVOKABLE bool checkDrawingKey(const QString &inputKey);

    // Сохраняет текущую информацию о пользователе в базе данных
    Q_INVOKABLE bool save();

    // Загружает информацию о пользователе из базы данных по логину
    Q_INVOKABLE bool loadUserFromDB(const QString &login);

    // Создает кошелек
    Q_INVOKABLE bool createWallet(int userId, const QString& walletName);

    // Получает кошелек пользователя
    Q_INVOKABLE QVariantList getWalletsByUser(int userId);

    // Считает количество кошельков у пользователя
    Q_INVOKABLE int countWalletsByUser(int userId);

    // Получает имя кошелька по его идентификатору для данного пользователя
    Q_INVOKABLE QString getWalletNameById(int userId, int walletId);

    // Получает доходы и расходы в этом месяце для каждого кошелька
    Q_INVOKABLE QVariantMap getMonthlyIncomeExpenses(QString walletName);

    // Обновляет выбранную тему приложения
    Q_INVOKABLE bool updatePalette(const QString &palette);

    // Обновляет цветовую палитру выбранную пользователем
    Q_INVOKABLE bool updateColorPalette(int userId, const QString& color_white, const QString& color_background_main,
                                        const QString& color_background_block, const QString& color_background_header,
                                        const QString& color_background_menu, const QString& color_transparent_button,
                                        const QString& color_kard_kategory, const QString& color_button,
                                        const QString& color_dark, const QString& color_black);

    // Получает цветовую палитру пользователя
    Q_INVOKABLE QVariantMap getUserColorPalette(int userId);

    // Получает цвета для палитры интерфейса
    Q_INVOKABLE QVariantMap getStandardPalette(const QString &palette);

    // Получает группу категорий пользователя
    Q_INVOKABLE QVariantList getCategoriesByUser(int userId);

    // Управляет данными БД
    Q_INVOKABLE void addDataDB(int userId);   
    Q_INVOKABLE bool deleteUserData(int userId);

    Q_INVOKABLE bool authenticateUserWithIp(const QString &login, const QString &password, const QString &ipAddress);
    Q_INVOKABLE bool isIpBanned(const QString &ipAddress);
    Q_INVOKABLE void incrementFailedLoginAttempts(const QString &ipAddress);
    Q_INVOKABLE void resetFailedLoginAttempts(const QString &ipAddress);
    Q_INVOKABLE void banIpAddress(const QString &ipAddress);

    Q_INVOKABLE bool changeUserPassword(const QString &oldPassword, const QString &newPassword);

    // Получает модель транзакций для этого пользователя
    Q_INVOKABLE TransactionModel* getTransactionModel() const;

signals:
    void userIdChanged();
    void firstNameChanged();
    void lastNameChanged();
    void emailChanged();
    void phoneChanged();
    void passwordChanged();
    void countryChanged();
    void currencyChanged();
    void colorPaletteChanged();
    void fontSizeChanged();
    void backgroundMusicChanged();
    void twoFactorAuthChanged();
    void twoFactorAuthChanged2();
    void photoChanged();
    void drawingKeyChanged();
    void walletCountChanged(int walletCount, int userId);
    void updateExpenseData();

private:
    QSqlDatabase db;
    bool createTables();
    bool openDatabase();
    QString m_userId;
    QString m_firstName;
    QString m_lastName;
    QString m_email;
    QString m_phone;
    QString m_password;
    QString m_countryUser;
    QString m_currencyUser;
    QString m_colorPalette;
    QString m_fontSize;
    QString m_backgroundMusic;
    QString m_twoFactorAuth;
    QString m_photo;
    QString m_drawing_key;

    TransactionModel* m_transactionModel;
};

#endif // DATABASEMANAGER_H
