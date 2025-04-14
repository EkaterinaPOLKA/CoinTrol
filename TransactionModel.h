#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>

class DatabaseManager;  // Предварительное объявление класса

class TransactionModel : public QAbstractListModel {
    Q_OBJECT

private:
    QSqlDatabase db;

    // Структура, представляющая одну транзакцию
    struct Transaction {
        int id;
        int userId;
        QString dateTime;
        QString status;
        QString type;
        double amount;
        double wasBalance;
        double newBalance;
        QString wallet;
        QString category;
        QString target;
        QString source;
        QString description;
    };

    // Структура, представляющая одну цель
    struct Target {
        int id;
        int userId;
        QString dateStart;
        QString nameTarget;
        double amount;
        double saved_amount;
        QString category;
        QString description;
        QString source1;
        QString source2;
        QString source3;
    };

    // Структура, представляющая одну цель из достигнутых
    struct TargetHistory {
        int id;
        int userId;
        int targetId;
        QString dateStart;
        QString nameTarget;
        double amount;
        double saved_amount;
        QString changeDate;
        QString category;
        QString description;
        QString source1;
        QString source2;
        QString source3;
    };

    // Список для хранения транзакций
    QList<Transaction> m_transactions;

    // Список для хранения целей
    QList<Target> m_targets;

    // Список для хранения целей
    QList<TargetHistory> m_targetsHistory;

    DatabaseManager* dbManager;

public:
    // Перечисление для задания пользовательских ролей модели
    enum TransactionRoles {
        IdRole = Qt::UserRole + 1,
        DateTimeRole,
        StatusRole,
        TypeRole,
        AmountRole,
        WasBalanceRole,
        NewBalanceRole,
        WalletRole,
        CategoryRole,
        SourceRole,
        DescriptionRole
    };

    // Конструктор
    explicit TransactionModel(DatabaseManager* dbManager, QObject* parent = nullptr);

    // Возвращает количество строк в модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Возвращает данные для элемента по заданному индексу
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Возвращает названия ролей, используемых в модели
    QHash<int, QByteArray> roleNames() const override;

    // Добавляет новую транзакцию в модель
    Q_INVOKABLE bool addTransaction(int userId, const QString &dateTime, const QString &status,
                                    const QString &type, double amount, const QString &wallet,
                                    const QString &category, const QString &target, const QString &source, const QString &description);

    // Публичный метод для перезагрузки транзакций для заданного пользователя
    Q_INVOKABLE void loadTransactions(int userId);

    // Получает все транзакции пользователя
    Q_INVOKABLE QVariantList getTransactionsByUser(int userId);

    // Возвращает количество транзакций для заданного пользователя
    Q_INVOKABLE int getTransactionCount(int userId);

    // Возвращает информацию о транзакции по индексу
    Q_INVOKABLE QVariantMap get(int index) const;

    // Удаляет транзакцию по индексу для заданного пользователя
    Q_INVOKABLE bool removeTransaction(int index, int userId);

    // Пересчитывает балансы с заданного индекса для пользователя и кошелька
    Q_INVOKABLE void recalculateBalances(int userId, const QString& wallet, int startIndex);

    // Сортирует транзакции по дате
    Q_INVOKABLE void sortByDate();

    // Сортировка по нескольким столбцам
    Q_INVOKABLE void sortByColumns(const QStringList &columns);

    // Обновляет транзакцию по её ID для заданного пользователя
    Q_INVOKABLE bool updateTransaction(int userId, int transactionId, const QString &dateTime, const QString &status,
                                       const QString &type, double amount, const QString &wallet,
                                       const QString &category, const QString &target, const QString &source, const QString &description);

    // Вкладка транзакции блок "Сводка транзакций за все время"
    Q_INVOKABLE int getTotalTransactionCount(int userId) const;
    Q_INVOKABLE double getTotalTransactionAmount(int userId) const;
    Q_INVOKABLE double getTransactionFrequency(int userId) const;
    Q_INVOKABLE int getTotalFailure(int userId) const;

    // Вкладка транзакции блок "Сводка транзакций за этот месяц"
    Q_INVOKABLE int getTotalTransactionCountForCurrentMonth(int userId) const;
    Q_INVOKABLE double getTotalTransactionAmountForCurrentMonth(int userId) const;
    Q_INVOKABLE double getTransactionFrequencyForCurrentMonth(int userId) const;
    Q_INVOKABLE int getTotalFailureForCurrentMonth(int userId) const;

    // Вкладка транзакции блок "Сводка транзакций за все время"
    Q_INVOKABLE double getTotalIncomeByUser(int userId) const;
    Q_INVOKABLE double getTotalExpenseByUser(int userId) const;
    Q_INVOKABLE double getTotalTaxesByUser(int userId) const;
    Q_INVOKABLE double getCleanIncomeByUser(int userId) const;

    // Вкладка транзакции блок "Сводка транзакций за этот месяц"
    Q_INVOKABLE double getTotalIncomeByUserForCurrentMonth(int userId) const;
    Q_INVOKABLE double getTotalExpenseByUserForCurrentMonth(int userId) const;
    Q_INVOKABLE double getTotalTaxesByUserForCurrentMonth(int userId) const;
    Q_INVOKABLE double getCleanIncomeByUserForCurrentMonth(int userId) const;

    // Обновляет баланс группы категорий
    Q_INVOKABLE void updateCategoryBalances(int userId, const QString& walletId, const QString &currentDateTime);

    // Обновляет баланс кошелька для пользователя
    Q_INVOKABLE bool updateWalletBalance(int userId, const QString& walletName);

    // Обновляет баланс кошелька после удаления транзакции
    Q_INVOKABLE bool updateWalletBalanceAfterRemoval(int userId, const QString& wallet, const QString& type, double amount);

    // Получает анализ транзакций
    Q_INVOKABLE QString getAnaliz(int userId, QString symbolValute) const;
    Q_INVOKABLE QString getAdvice(int userId, QString symbolValute) const;

    // Вкладка планы
    Q_INVOKABLE void addPlan(const int userId, const QString &type, const QString &name, const QString &amount, const QString &description);
    Q_INVOKABLE QVariantList fetchPlans(const int userId, const QString &type);
    Q_INVOKABLE bool removePlan(const int userId, const QString &planName, const QString &planType);

    // Вкладка цели
    Q_INVOKABLE bool addTarget(const int userId, const QString &dateStart, const QString &nameT, const double &amount, const double &saveAmount, const QString &category, const QString &description, const QString &source1, const QString &source2, const QString &source3);
    Q_INVOKABLE QVariantList fetchTargets(const int userId);
    Q_INVOKABLE bool editTarget(const int userId, const int currentId, const QString &dateStart, const QString &nameT, const double &amount, const double &saveAmount, const QString &category, const QString &description, const QString &source1, const QString &source2, const QString &source3);
    Q_INVOKABLE bool removeTarget(const int userId, const int index);
    Q_INVOKABLE QVariantList fetchTargetsHistory(const int userId);
    Q_INVOKABLE void moveTargetToHistory(int userId, const QString& targetName);

    // Удаляет кошелек
    Q_INVOKABLE bool removeWallet(const int userId,  const QString &nameWallet);

    // Вызывает сигналы для изменения символа валюты везде, где упоминается
    Q_INVOKABLE void getValuteAll(const int userId);

    // Преобразует фото в бинарный код
    Q_INVOKABLE QString reversePhoto(QString selectedFile);

signals:
    void transactionCountChanged();
    void planAdded();
    void planRemoved();
    void targetAdded();
    void targetEdited();
    void targetRemoved();
};

#endif // TRANSACTIONMODEL_H
