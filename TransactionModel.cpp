#include "TransactionModel.h"
#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QFile>
#include <QFileInfo>

TransactionModel::TransactionModel(DatabaseManager* dbManager, QObject* parent)
    : QAbstractListModel(parent), dbManager(dbManager) {
    db = dbManager->database();
    if (!db.isOpen()) {
        qDebug() << "Ошибка: соединение с базой данных не установлено!";
    }
}

int TransactionModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_transactions.count();
}

QVariant TransactionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_transactions.count()) {
        return QVariant();
    }

    const Transaction &transaction = m_transactions[index.row()];

    switch (role) {
    case IdRole:
        return transaction.id;
    case DateTimeRole:
        return transaction.dateTime;
    case StatusRole:
        return transaction.status;
    case TypeRole:
        return transaction.type;
    case AmountRole:
        return QString::number(transaction.amount, 'f', 2);
    case WasBalanceRole:
        return QString::number(transaction.wasBalance, 'f', 2);
    case NewBalanceRole:
        return QString::number(transaction.newBalance, 'f', 2);
    case WalletRole:
        return transaction.wallet;
    case CategoryRole:
        return transaction.category;
    case SourceRole:
        return transaction.source;
    case DescriptionRole:
        return transaction.description;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TransactionModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[DateTimeRole] = "dateTime";
    roles[StatusRole] = "status";
    roles[TypeRole] = "type";
    roles[AmountRole] = "amount";
    roles[WasBalanceRole] = "wasBalance";
    roles[NewBalanceRole] = "newBalance";
    roles[WalletRole] = "wallet";
    roles[CategoryRole] = "category";
    roles[SourceRole] = "source";
    roles[DescriptionRole] = "description";
    return roles;
}

void TransactionModel::loadTransactions(int userId) {
    beginResetModel();
    m_transactions.clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM transactions WHERE user_id = :userId ORDER BY date_time DESC");
    query.bindValue(":userId", userId);

    if (query.exec()) {
        while (query.next()) {
            Transaction transaction;
            transaction.id = query.value("id").toInt();
            transaction.userId = userId;
            transaction.dateTime = query.value("date_time").toString();
            transaction.status = query.value("status").toString();
            transaction.type = query.value("type").toString();
            transaction.amount = query.value("amount").toDouble();
            transaction.wasBalance = query.value("previous_balance").toDouble();
            transaction.newBalance = query.value("new_balance").toDouble();
            transaction.wallet = query.value("wallet").toString();
            transaction.category = query.value("category").toString();
            transaction.source = query.value("source").toString();
            transaction.description = query.value("description").toString();

            qDebug() << "Transaction ID:" << transaction.id
                     << "User ID:" << transaction.userId
                     << "DateTime:" << transaction.dateTime
                     << "Status:" << transaction.status
                     << "Type:" << transaction.type
                     << "Amount:" << transaction.amount
                     << "Previous Balance:" << transaction.wasBalance
                     << "New Balance:" << transaction.newBalance
                     << "Wallet:" << transaction.wallet
                     << "Category:" << transaction.category
                     << "Description:" << transaction.description;

            m_transactions.append(transaction);
        }
    } else {
        qDebug() << "Ошибка: не удалось загрузить транзакции." << query.lastError().text();
    }

    endResetModel();
    emit transactionCountChanged();
    emit dbManager->updateExpenseData();
}

QVariantList TransactionModel::getTransactionsByUser(int userId) {
    QVariantList transactionsList;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId == userId) {
            QVariantMap transactionMap;
            transactionMap["id"] = transaction.id;
            transactionMap["dateTime"] = transaction.dateTime;
            transactionMap["status"] = transaction.status;
            transactionMap["type"] = transaction.type;
            transactionMap["amount"] = transaction.amount;
            transactionMap["wallet"] = transaction.wallet;
            transactionMap["category"] = transaction.category;
            transactionMap["source"] = transaction.source;
            transactionMap["description"] = transaction.description;

            transactionsList.append(transactionMap);
        }
    }

    return transactionsList;
}

int TransactionModel::getTransactionCount(int userId) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM transactions WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (m_transactions.isEmpty()) {
        emit transactionCountChanged();
        return 0;
    }

    if (!query.exec()) {
        qWarning() << "Error executing query:" << query.lastError().text();
        emit transactionCountChanged();
        return 0;
    }

    if (!query.next()) {
        qWarning() << "Error retrieving query result:" << query.lastError().text();
        emit transactionCountChanged();
        return 0;
    }

    return query.value(0).toInt();
}

QVariantMap TransactionModel::get(int index) const {
    QVariantMap transactionMap;
    if (index >= 0 && index < m_transactions.size()) {
        const Transaction &transaction = m_transactions[index];
        transactionMap["id"] = transaction.id;
        transactionMap["dateTime"] = transaction.dateTime;
        transactionMap["status"] = transaction.status;
        transactionMap["type"] = transaction.type;
        transactionMap["amount"] = QString::number(transaction.amount, 'f', 2);
        transactionMap["wasBalance"] = QString::number(transaction.wasBalance, 'f', 2);
        transactionMap["newBalance"] = QString::number(transaction.newBalance, 'f', 2);
        transactionMap["wallet"] = transaction.wallet;
        transactionMap["category"] = transaction.category;
        transactionMap["source"] = transaction.source;
        transactionMap["description"] = transaction.description;
    }
    return transactionMap;
}

int TransactionModel::getTotalTransactionCount(int userId) const {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM transactions WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qWarning() << "Ошибка выполнения запроса:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

double TransactionModel::getTotalTransactionAmount(int userId) const {
    double totalBalance = 0.0;

    QSqlQuery query;
    query.prepare("SELECT SUM(balance) "
                  "FROM wallets "
                  "WHERE user_id = :userId");
    query.bindValue(":userId", userId);

    if (query.exec() && query.next()) {
        totalBalance = query.value(0).toDouble();
    } else {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
    }

    return totalBalance;
}

double TransactionModel::getTransactionFrequency(int userId) const {
    if (m_transactions.isEmpty()) return 0.0;

    QMap<QString, int> monthTransactionCount;

    for (const auto &transaction : m_transactions) {
        QDate date = QDate::fromString(transaction.dateTime.split(' ').first(), "dd.MM.yyyy");
        QString monthYear = QString::number(date.year()) + "-" + QString::number(date.month());

        monthTransactionCount[monthYear]++;
    }

    int totalTransactions = 0;
    int totalMonths = monthTransactionCount.size();

    for (auto count : monthTransactionCount) {
        totalTransactions += count;
    }

    return totalMonths > 0 ? static_cast<double>(totalTransactions) / totalMonths : 0.0;
}

int TransactionModel::getTotalFailure(int userId) const{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM transactions WHERE user_id = :user_id AND status != 'Выполнено'");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qWarning() << "Ошибка выполнения запроса:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

int TransactionModel::getTotalTransactionCountForCurrentMonth(int userId) const {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    int count = 0;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId != userId) {
            continue;
        }

        QDateTime dateTime = QDateTime::fromString(transaction.dateTime, "dd.MM.yyyy HH:mm:ss");
        if (!dateTime.isValid()) {
            qWarning() << "Неверный формат dateTime:" << transaction.dateTime;
            continue;
        }

        QDate transactionDate = dateTime.date();
        int transactionMonth = transactionDate.month();
        int transactionYear = transactionDate.year();

        if (transactionMonth == currentMonth && transactionYear == currentYear) {
            ++count;
        }
    }

    return count;
}

double TransactionModel::getTotalTransactionAmountForCurrentMonth(int userId) const {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    double totalAmount = 0.0;
    int transactionCount = 0;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId != userId) {
            continue;
        }

        QDateTime dateTime = QDateTime::fromString(transaction.dateTime, "dd.MM.yyyy HH:mm:ss");
        if (!dateTime.isValid()) {
            qWarning() << "Неверный формат dateTime:" << transaction.dateTime;
            continue;
        }

        QDate transactionDate = dateTime.date();
        if (transactionDate.month() == currentMonth && transactionDate.year() == currentYear && transaction.status == "Выполнено" && transaction.type == "Расход") {
            totalAmount += transaction.amount;
            transactionCount++;
        }
    }

    if (transactionCount == 0) {
        return 0.0;
    }

    return totalAmount / transactionCount;
}

double TransactionModel::getTransactionFrequencyForCurrentMonth(int userId) const {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    int totalTransactions = 0;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId != userId) {
            continue;
        }

        QDateTime dateTime = QDateTime::fromString(transaction.dateTime, "dd.MM.yyyy HH:mm:ss");
        if (!dateTime.isValid()) {
            qWarning() << "Неверный формат dateTime:" << transaction.dateTime;
            continue;
        }

        QDate transactionDate = dateTime.date();
        if (transactionDate.month() == currentMonth && transactionDate.year() == currentYear) {
            ++totalTransactions;
        }
    }

    int daysInMonth = currentDate.daysInMonth();
    int totalWeeks = (daysInMonth + currentDate.dayOfWeek() - 1) / 7;

    return totalWeeks > 0 ? static_cast<double>(totalTransactions) / totalWeeks : 0.0;
}

int TransactionModel::getTotalFailureForCurrentMonth(int userId) const {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    int failureCount = 0;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId != userId) {
            continue;
        }

        QDateTime dateTime = QDateTime::fromString(transaction.dateTime, "dd.MM.yyyy HH:mm:ss");
        if (!dateTime.isValid()) {
            qWarning() << "Неверный формат dateTime:" << transaction.dateTime;
            continue;
        }

        QDate transactionDate = dateTime.date();
        if (transactionDate.month() == currentMonth && transactionDate.year() == currentYear && transaction.status != "Выполнено") {
            ++failureCount;
        }
    }

    return failureCount;
}

void TransactionModel::sortByDate() {
    beginResetModel();
    std::sort(m_transactions.begin(), m_transactions.end(), [](const Transaction &a, const Transaction &b) {
        QDateTime dateTimeA = QDateTime::fromString(a.dateTime, "dd.MM.yyyy HH:mm:ss");
        QDateTime dateTimeB = QDateTime::fromString(b.dateTime, "dd.MM.yyyy HH:mm:ss");

        if (!dateTimeA.isValid() || !dateTimeB.isValid()) {
            qWarning() << "Неверный формат даты для транзакции." << a.dateTime << b.dateTime;
            return false;
        }
        return dateTimeA < dateTimeB;
    });
    endResetModel();
}

void TransactionModel::sortByColumns(const QStringList &columns) {
    if (columns.isEmpty()) return;

    beginResetModel();

    std::sort(m_transactions.begin(), m_transactions.end(), [&](const Transaction &a, const Transaction &b) {
        for (const QString &column : columns) {
            if (column == "dateTime") {
                QDateTime dateTimeA = QDateTime::fromString(a.dateTime, "dd.MM.yyyy HH:mm:ss");
                QDateTime dateTimeB = QDateTime::fromString(b.dateTime, "dd.MM.yyyy HH:mm:ss");
                if (dateTimeA != dateTimeB) {
                    return dateTimeA > dateTimeB;
                }
            } else if (column == "status") {
                if (a.status != b.status) {
                    return a.status < b.status;
                }
            } else if (column == "type") {
                if (a.type != b.type) {
                    return a.type < b.type;
                }
            } else if (column == "amount") {
                if (a.amount != b.amount) {
                    return a.amount < b.amount;
                }
            } else if (column == "wasBalance") {
                if (a.wasBalance != b.wasBalance) {
                    return a.wasBalance < b.wasBalance;
                }
            } else if (column == "newBalance") {
                if (a.newBalance != b.newBalance) {
                    return a.newBalance < b.newBalance;
                }
            } else if (column == "wallet") {
                if (a.wallet != b.wallet) {
                    return a.wallet < b.wallet;
                }
            } else if (column == "category") {
                if (a.category != b.category) {
                    return a.category < b.category;
                }
            }
        }
        return false;
    });

    endResetModel();
}

void TransactionModel::updateCategoryBalances(int userId, const QString& walletId, const QString& currentDateTime) {
    QMap<QString, double> categoryBalances;
    double totalExpenses = 0.0;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId == userId && transaction.wallet == walletId &&
            transaction.type == "Расход" && transaction.status == "Выполнено") {
            categoryBalances[transaction.category] += transaction.amount;
            totalExpenses += transaction.amount;
        }
    }

    for (auto it = categoryBalances.begin(); it != categoryBalances.end(); ++it) {
        QString categoryName = it.key();
        double balance = it.value();
        double percentage = (totalExpenses > 0) ? (balance / totalExpenses) * 100 : 0;

        QString link = QString("../Source/%1.svg").arg(categoryName);

        QSqlQuery query;
        query.prepare("INSERT INTO categories (user_id, wallet_id, group_name, link, value, percentage, date_time) "
                      "VALUES (:user_id, :wallet_id, :group_name, :link, :value, :percentage, :date_time) "
                      "ON CONFLICT(user_id, wallet_id, group_name) DO UPDATE SET value = :value, percentage = :percentage, date_time = :date_time");
        query.bindValue(":user_id", userId);
        query.bindValue(":wallet_id", walletId);
        query.bindValue(":group_name", categoryName);
        query.bindValue(":link", link);
        query.bindValue(":value", balance);
        query.bindValue(":percentage", percentage);
        query.bindValue(":date_time", currentDateTime);

        if (!query.exec()) {
            qDebug() << "Ошибка обновления категории:" << query.lastError().text();
        }
    }

    QSet<QString> categoriesInDb;
    QSqlQuery categoryQuery;
    categoryQuery.prepare("SELECT group_name FROM categories WHERE user_id = :user_id AND wallet_id = :wallet_id");
    categoryQuery.bindValue(":user_id", userId);
    categoryQuery.bindValue(":wallet_id", walletId);

    if (categoryQuery.exec()) {
        while (categoryQuery.next()) {
            categoriesInDb.insert(categoryQuery.value(0).toString());
        }
    } else {
        qDebug() << "Ошибка получения категорий из БД:" << categoryQuery.lastError().text();
    }

    for (const QString &categoryName : categoriesInDb) {
        if (!categoryBalances.contains(categoryName)) {
            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM categories WHERE user_id = :user_id AND wallet_id = :wallet_id AND group_name = :group_name");
            deleteQuery.bindValue(":user_id", userId);
            deleteQuery.bindValue(":wallet_id", walletId);
            deleteQuery.bindValue(":group_name", categoryName);

            if (!deleteQuery.exec()) {
                qDebug() << "Ошибка удаления категории:" << deleteQuery.lastError().text();
            }
        }
    }

    emit dbManager->updateExpenseData();
}

double TransactionModel::getTotalIncomeByUser(int userId) const {
    double totalIncome = 0.0;
    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId == userId && transaction.status == "Выполнено" && transaction.type == "Доход") {
            totalIncome += transaction.amount;
        }
    }
    return totalIncome;
}

double TransactionModel::getTotalExpenseByUser(int userId) const {
    double totalExpense = 0.0;
    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId == userId && transaction.status == "Выполнено" && transaction.type == "Расход") {
            totalExpense += transaction.amount;
        }
    }
    return totalExpense;
}

double TransactionModel::getTotalTaxesByUser(int userId) const {
    double totalNonTaxExpenses = 0.0;
    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId == userId && transaction.status == "Выполнено" &&
            transaction.type == "Расход" && transaction.category == "Налоги") {
            totalNonTaxExpenses += transaction.amount;
        }
    }
    return totalNonTaxExpenses;
}

double TransactionModel::getCleanIncomeByUser(int userId) const {
    double totalIncome = getTotalIncomeByUser(userId);
    double totalExpense = getTotalExpenseByUser(userId);
    double totalNonTaxExpenses = getTotalTaxesByUser(userId);

    return totalIncome - totalExpense - totalNonTaxExpenses;
}

double TransactionModel::getTotalIncomeByUserForCurrentMonth(int userId) const {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    double totalIncome = 0.0;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId != userId) {
            continue;
        }

        QDateTime dateTime = QDateTime::fromString(transaction.dateTime, "dd.MM.yyyy HH:mm:ss");
        if (!dateTime.isValid()) {
            qWarning() << "Неверный формат dateTime:" << transaction.dateTime;
            continue;
        }

        QDate transactionDate = dateTime.date();
        if (transactionDate.month() == currentMonth && transactionDate.year() == currentYear &&
            transaction.status == "Выполнено" && transaction.type == "Доход") {
            totalIncome += transaction.amount;
        }
    }

    return totalIncome;
}

double TransactionModel::getTotalExpenseByUserForCurrentMonth(int userId) const {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    double totalExpense = 0.0;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId != userId) {
            continue;
        }

        QDateTime dateTime = QDateTime::fromString(transaction.dateTime, "dd.MM.yyyy HH:mm:ss");
        if (!dateTime.isValid()) {
            qWarning() << "Неверный формат dateTime:" << transaction.dateTime;
            continue;
        }

        QDate transactionDate = dateTime.date();
        if (transactionDate.month() == currentMonth && transactionDate.year() == currentYear &&
            transaction.status == "Выполнено" && transaction.type == "Расход") {
            totalExpense += transaction.amount;
        }
    }
    return totalExpense;
}

double TransactionModel::getTotalTaxesByUserForCurrentMonth(int userId) const {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    double totalNonTaxExpenses = 0.0;

    for (const Transaction &transaction : m_transactions) {
        if (transaction.userId != userId) {
            continue;
        }

        QDateTime dateTime = QDateTime::fromString(transaction.dateTime, "dd.MM.yyyy HH:mm:ss");
        if (!dateTime.isValid()) {
            qWarning() << "Неверный формат dateTime:" << transaction.dateTime;
            continue;
        }

        QDate transactionDate = dateTime.date();
        if (transactionDate.month() == currentMonth && transactionDate.year() == currentYear &&
            transaction.status == "Выполнено" && transaction.type == "Расход" && transaction.category == "Налоги") {
            totalNonTaxExpenses += transaction.amount;
        }
    }

    return totalNonTaxExpenses;
}

double TransactionModel::getCleanIncomeByUserForCurrentMonth(int userId) const {
    double totalIncome = getTotalIncomeByUserForCurrentMonth(userId);
    double totalExpense = getTotalExpenseByUserForCurrentMonth(userId);
    double totalNonTaxExpenses = getTotalTaxesByUserForCurrentMonth(userId);

    return totalIncome - totalExpense - totalNonTaxExpenses;
}

bool TransactionModel::removeWallet(const int userId, const QString &nameWallet) {
    if (nameWallet.isEmpty()) {
        qDebug() << "Ошибка: некорректное имя кошелька.";
        return false;
    }

    if (!db.transaction()) {
        qDebug() << "Ошибка: не удалось начать транзакцию." << db.lastError().text();
        return false;
    }

    QSqlQuery deleteTransactionsQuery;
    deleteTransactionsQuery.prepare("DELETE FROM transactions WHERE user_id = :user_id AND wallet = :wallet_name");
    deleteTransactionsQuery.bindValue(":user_id", userId);
    deleteTransactionsQuery.bindValue(":wallet_name", nameWallet);

    if (!deleteTransactionsQuery.exec()) {
        qDebug() << "Ошибка при удалении транзакций:" << deleteTransactionsQuery.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery deleteWalletQuery;
    deleteWalletQuery.prepare("DELETE FROM wallets WHERE user_id = :user_id AND wallet_name = :wallet_name");
    deleteWalletQuery.bindValue(":user_id", userId);
    deleteWalletQuery.bindValue(":wallet_name", nameWallet);

    if (!deleteWalletQuery.exec()) {
        qDebug() << "Ошибка при удалении кошелька:" << deleteWalletQuery.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qDebug() << "Ошибка при подтверждении транзакции:" << db.lastError().text();
        return false;
    }

    loadTransactions(userId);

    emit transactionCountChanged();
    emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);
    emit dbManager->updateExpenseData();

    return true;
}

void TransactionModel::getValuteAll(const int userId) {
    emit transactionCountChanged();
    emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);
    emit dbManager->updateExpenseData();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////  ТРАНЗАКЦИИ  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TransactionModel::addTransaction(int userId, const QString &dateTime, const QString &status,
                                      const QString &type, double amount, const QString &wallet,
                                      const QString &category, const QString &target, const QString &source, const QString &description) {
    double previousBalance = 0.0;
    double newBalance = 0.0;

    for (const Transaction &transaction : qAsConst(m_transactions)) {
        if (transaction.wallet == wallet) {
            previousBalance = transaction.newBalance;
        }
    }

    if (status == "Выполнено") {
        if (type == "Доход") {
            newBalance = previousBalance + amount;
        } else if (type == "Расход") {
            newBalance = previousBalance - amount;
            if (newBalance < 0) {
                qWarning() << "Ошибка: добавление расходной транзакции приведет к отрицательному балансу.";
                return false;
            }
        }
    } else {
        newBalance = previousBalance;
    }

    Transaction newTransaction;
    newTransaction.userId = userId;
    newTransaction.dateTime = dateTime;
    newTransaction.status = status;
    newTransaction.type = type;
    newTransaction.amount = amount;
    newTransaction.wasBalance = previousBalance;
    newTransaction.newBalance = newBalance;
    newTransaction.wallet = wallet;
    newTransaction.category = category;
    newTransaction.target = target;
    newTransaction.source = source;
    newTransaction.description = description;

    QSqlQuery query;
    query.prepare("INSERT INTO transactions (user_id, date_time, status, type, amount, previous_balance, new_balance, wallet, category, target, source, description) "
                  "VALUES (:user_id, :date_time, :status, :type, :amount, :previous_balance, :new_balance, :wallet, :category, :target, :source, :description)");

    query.bindValue(":user_id", userId);
    query.bindValue(":date_time", newTransaction.dateTime);
    query.bindValue(":status", newTransaction.status);
    query.bindValue(":type", newTransaction.type);
    query.bindValue(":amount", newTransaction.amount);
    query.bindValue(":previous_balance", newTransaction.wasBalance);
    query.bindValue(":new_balance", newTransaction.newBalance);
    query.bindValue(":wallet", newTransaction.wallet);
    query.bindValue(":category", newTransaction.category);
    query.bindValue(":target", newTransaction.target);
    query.bindValue(":source", newTransaction.source);
    query.bindValue(":description", newTransaction.description);

    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении транзакции:" << query.lastError().text();
        return false;
    }

    if (!updateWalletBalance(userId, wallet)) {
        qDebug() << "Ошибка: не удалось обновить баланс кошелька.";
        return false;
    }

    m_transactions.append(newTransaction);

    if(newTransaction.status == "Выполнено") {
        updateCategoryBalances(userId, wallet, dateTime);
    }

    if (category == "Цель" && !target.isEmpty() && newTransaction.status == "Выполнено") {
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE targets SET saved_amount = saved_amount + :amount WHERE user_id = :user_id AND target_name = :target_name");
        updateQuery.bindValue(":amount", amount);
        updateQuery.bindValue(":user_id", userId);
        updateQuery.bindValue(":target_name", target);

        if (!updateQuery.exec()) {
            qDebug() << "Ошибка при обновлении saved_amount для цели:" << updateQuery.lastError().text();
            return false;
        }

        // Проверка, достигнута ли цель
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT target_amount, saved_amount FROM targets WHERE user_id = :user_id AND target_name = :target_name");
        checkQuery.bindValue(":user_id", userId);
        checkQuery.bindValue(":target_name", target);

        if (checkQuery.exec() && checkQuery.next()) {
            double targetAmount = checkQuery.value(0).toDouble();
            double savedAmount = checkQuery.value(1).toDouble();

            // Если цель достигнута, переместить её в историю
            if (savedAmount >= targetAmount) {
                moveTargetToHistory(userId, target);
            }
        }
    }


    emit transactionCountChanged();
    emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);

    return true;
}

bool TransactionModel::removeTransaction(int index, int userId) {
    if (index < 0 || index >= m_transactions.size()) {
        qDebug() << "Error: Invalid transaction index.";
        return false;
    }

    Transaction transactionToRemove = m_transactions.at(index);

    if (!db.transaction()) {
        qDebug() << "Error: Unable to start a database transaction." << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM transactions WHERE id = :id AND user_id = :user_id");
    query.bindValue(":id", transactionToRemove.id);
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qDebug() << "Error: Failed to delete transaction from database." << query.lastError().text();
        db.rollback();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qDebug() << "Error: Transaction was not found in the database.";
        db.rollback();
        return false;
    }

    if (transactionToRemove.category == "Цель" && transactionToRemove.status == "Выполнено") {
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE targets SET saved_amount = saved_amount - :amount WHERE user_id = :user_id AND target_name = :target_name");
        updateQuery.bindValue(":amount", transactionToRemove.amount);
        updateQuery.bindValue(":user_id", userId);
        updateQuery.bindValue(":target_name", transactionToRemove.target);

        if (!updateQuery.exec()) {
            qDebug() << "Ошибка при обновлении saved_amount для цели:" << updateQuery.lastError().text();
            db.rollback();
            return false;
        }
    }

    if (!updateWalletBalanceAfterRemoval(userId, transactionToRemove.wallet, transactionToRemove.type, transactionToRemove.amount)) {
        qDebug() << "Ошибка: Не удалось обновить баланс кошелька после удаления транзакции.";
        db.rollback();
        return false;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_transactions.removeAt(index);
    endRemoveRows();

    updateCategoryBalances(userId, transactionToRemove.wallet, transactionToRemove.dateTime);

    if (!db.commit()) {
        qDebug() << "Error: Unable to commit the database transaction." << db.lastError().text();
        return false;
    }

    emit transactionCountChanged();
    emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);
    emit dbManager->updateExpenseData();

    return true;
}

bool TransactionModel::updateTransaction(int userId, int transactionId, const QString &dateTime,
                                         const QString &newStatus, const QString &type, double amount,
                                         const QString &wallet, const QString &category, const QString &target,
                                         const QString &source, const QString &description) {
    int index = -1;
    Transaction oldTransaction;

    for (int i = 0; i < m_transactions.size(); ++i) {
        if (m_transactions[i].id == transactionId) {
            index = i;
            oldTransaction = m_transactions[i];
            break;
        }
    }

    if (index == -1) {
        qDebug() << "Ошибка: транзакция не найдена.";
        return false;
    }

    double oldWalletBalance = oldTransaction.wasBalance;
    double newWalletBalance = 0.0;
    double previousBalanceOldWallet = 0.0;
    double previousBalanceNewWallet = 0.0;

    for (const Transaction &transaction : qAsConst(m_transactions)) {
        if (transaction.wallet == oldTransaction.wallet && transaction.status == "Выполнено") {
            previousBalanceOldWallet = transaction.newBalance;
        } else {
            previousBalanceOldWallet = oldWalletBalance;
        }
    }

    if (wallet != oldTransaction.wallet) {
        for (const Transaction &transaction : qAsConst(m_transactions)) {
            if (transaction.wallet == wallet && transaction.status == "Выполнено") {
                previousBalanceNewWallet = transaction.newBalance;
            } else {
                previousBalanceNewWallet = oldWalletBalance;
            }
        }
    }

    Transaction &transaction = m_transactions[index];
    transaction.dateTime = dateTime;
    transaction.status = newStatus;
    transaction.type = type;
    transaction.amount = amount;
    transaction.wallet = wallet;
    transaction.category = category;
    transaction.target = target;
    transaction.source = source;
    transaction.description = description;

    if (newStatus == "Выполнено") {
        if (type == "Доход") {
            newWalletBalance = (wallet == oldTransaction.wallet) ? previousBalanceOldWallet + amount : previousBalanceNewWallet + amount;
        } else {
            newWalletBalance = (wallet == oldTransaction.wallet) ? previousBalanceOldWallet - amount : previousBalanceNewWallet - amount;
            if (newWalletBalance < 0) {
                qWarning() << "Ошибка: обновление транзакции приведет к отрицательному балансу.";
                return false;
            }
        }
    } else {
        newWalletBalance = (wallet == oldTransaction.wallet) ? previousBalanceOldWallet : previousBalanceNewWallet;
    }

    transaction.wasBalance = (wallet == oldTransaction.wallet) ? previousBalanceOldWallet : previousBalanceNewWallet;
    transaction.newBalance = newWalletBalance;

    QSqlQuery query;
    query.prepare("UPDATE transactions SET date_time = :date_time, status = :status, type = :type, amount = :amount, "
                  "wallet = :wallet, category = :category, target = :target, source = :source, description = :description, "
                  "previous_balance = :previous_balance, new_balance = :new_balance "
                  "WHERE id = :transaction_id AND user_id = :user_id");

    query.bindValue(":date_time", dateTime);
    query.bindValue(":status", newStatus);
    query.bindValue(":type", type);
    query.bindValue(":amount", amount);
    query.bindValue(":wallet", wallet);
    query.bindValue(":category", category);
    query.bindValue(":target", target);
    query.bindValue(":source", source);
    query.bindValue(":description", description);
    query.bindValue(":previous_balance", transaction.wasBalance);
    query.bindValue(":new_balance", transaction.newBalance);
    query.bindValue(":transaction_id", transactionId);
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qDebug() << "Ошибка: не удалось обновить транзакцию в базе данных." << query.lastError().text();
        return false;
    }

    recalculateBalances(userId, wallet, index);
    if (wallet != oldTransaction.wallet) {
        recalculateBalances(userId, oldTransaction.wallet, index);
    }

    if (!updateWalletBalance(userId, wallet)) {
        qDebug() << "Ошибка: не удалось обновить баланс кошелька после изменения транзакции.";
        return false;
    }

    if (wallet != oldTransaction.wallet) {
        if (!updateWalletBalance(userId, oldTransaction.wallet)) {
            qDebug() << "Ошибка: не удалось обновить баланс старого кошелька.";
            return false;
        }
    }

    updateCategoryBalances(userId, oldTransaction.wallet, dateTime);
    updateCategoryBalances(userId, wallet, dateTime);

    if (category == "Цель" && target != "") {
        double difference = amount - oldTransaction.amount;
        QSqlQuery updateQuery;

        updateQuery.prepare("UPDATE targets SET saved_amount = saved_amount + :difference WHERE user_id = :user_id AND target_name = :target_name");
        updateQuery.bindValue(":difference", difference);
        updateQuery.bindValue(":user_id", userId);
        updateQuery.bindValue(":target_name", target);

        if (!updateQuery.exec()) {
            qDebug() << "Ошибка при обновлении saved_amount для цели:" << updateQuery.lastError().text();
            return false;
        }
    }

    emit transactionCountChanged();
    emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);
    emit dbManager->updateExpenseData();

    return true;
}

void TransactionModel::recalculateBalances(int userId, const QString &wallet, int startIndex) {
    if (startIndex < 0 || startIndex >= m_transactions.size()) {
        emit transactionCountChanged();
        emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);
        emit dbManager->updateExpenseData();
        return;
    }

    double previousBalance = 0.0;

    for (int i = 0; i < m_transactions.size(); ++i) {
        Transaction &transaction = m_transactions[i];

        if (transaction.wallet == wallet) {
            if (i < startIndex) {
                transaction.wasBalance = previousBalance;
                previousBalance = transaction.newBalance;
            } else if (i >= startIndex) {
                transaction.wasBalance = previousBalance;

                if (transaction.status == "Выполнено") {
                    if (transaction.type == "Доход") {
                        transaction.newBalance = previousBalance + transaction.amount;
                    } else if (transaction.type == "Расход") {
                        transaction.newBalance = previousBalance - transaction.amount;
                    }
                } else {
                    transaction.newBalance = previousBalance;
                }

                previousBalance = transaction.newBalance;

                QSqlQuery updateQuery;
                updateQuery.prepare("UPDATE transactions SET previous_balance = :previous_balance, new_balance = :new_balance WHERE id = :id");
                updateQuery.bindValue(":previous_balance", transaction.wasBalance);
                updateQuery.bindValue(":new_balance", transaction.newBalance);
                updateQuery.bindValue(":id", transaction.id);

                if (!updateQuery.exec()) {
                    qDebug() << "Ошибка: не удалось обновить баланс транзакции." << updateQuery.lastError().text();
                }

                QModelIndex modelIndex = createIndex(i, 0);
                emit transactionCountChanged();
                emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);
                emit dbManager->updateExpenseData();
                emit dataChanged(modelIndex, modelIndex);
            }
        }
    }
}

bool TransactionModel::updateWalletBalance(int userId, const QString& walletName) {
    db.transaction();

    QSqlQuery totalBalanceQuery;
    totalBalanceQuery.prepare(R"(
        SELECT SUM(
            CASE WHEN type = 'Доход' THEN amount
                 WHEN type = 'Расход' THEN -amount
                 ELSE 0
            END
        ) AS total_balance
        FROM transactions
        WHERE user_id = :user_id
          AND wallet = :wallet_name
          AND status = 'Выполнено'
    )");
    totalBalanceQuery.bindValue(":user_id", userId);
    totalBalanceQuery.bindValue(":wallet_name", walletName);

    double finalBalance = 0.0;
    if (totalBalanceQuery.exec()) {
        if (totalBalanceQuery.next()) {
            finalBalance = totalBalanceQuery.value(0).toDouble();
        } else {
            qDebug() << "Нет завершенных транзакций для данного кошелька.";
            db.rollback();
            return false;
        }
    } else {
        qDebug() << "Ошибка при получении итогового баланса кошелька:" << totalBalanceQuery.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE wallets SET balance = :balance WHERE user_id = :user_id AND wallet_name = :wallet_name");
    updateQuery.bindValue(":balance", finalBalance);
    updateQuery.bindValue(":user_id", userId);
    updateQuery.bindValue(":wallet_name", walletName);

    if (!updateQuery.exec()) {
        qDebug() << "Ошибка при обновлении баланса кошелька:" << updateQuery.lastError().text();
        db.rollback();
        return false;
    }

    db.commit();

    emit transactionCountChanged();
    emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);
    emit dbManager->updateExpenseData();
    return true;
}

bool TransactionModel::updateWalletBalanceAfterRemoval(int userId, const QString& wallet, const QString& type, double amount) {
    double newWalletBalance = 0.0;

    QSqlQuery lastTransactionQuery;
    lastTransactionQuery.prepare(R"(
        SELECT new_balance
        FROM transactions
        WHERE user_id = :user_id
          AND wallet = :wallet_name
          AND status = 'Выполнено'
        ORDER BY date_time DESC
        LIMIT 1
    )");
    lastTransactionQuery.bindValue(":user_id", userId);
    lastTransactionQuery.bindValue(":wallet_name", wallet);

    if (lastTransactionQuery.exec() && lastTransactionQuery.next()) {
        newWalletBalance = lastTransactionQuery.value(0).toDouble();

        if (type == "Доход") {
            newWalletBalance -= amount;
        } else if (type == "Расход") {
            newWalletBalance += amount;
        }
    } else {
        newWalletBalance = 0.0;
    }

    QSqlQuery updateWalletQuery;
    updateWalletQuery.prepare("UPDATE wallets SET balance = :balance WHERE user_id = :user_id AND wallet_name = :wallet_name");
    updateWalletQuery.bindValue(":balance", newWalletBalance);
    updateWalletQuery.bindValue(":user_id", userId);
    updateWalletQuery.bindValue(":wallet_name", wallet);

    if (!updateWalletQuery.exec()) {
        qDebug() << "Ошибка при обновлении баланса кошелька:" << updateWalletQuery.lastError().text();
        return false;
    }

    emit transactionCountChanged();
    emit dbManager->walletCountChanged(dbManager->countWalletsByUser(userId), userId);
    emit dbManager->updateExpenseData();

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////  АНАЛИЗ  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString TransactionModel::getAnaliz(int userId, QString symbolValute) const {
    QHash<QString, double> categoryExpenses;
    QHash<QString, double> incomeSources;
    QHash<QString, int> categoryCounts;

    QHash<QString, double> monthlyExpenses;
    QHash<QString, double> monthlyIncomes;

    QDate currentDate = QDate::currentDate();
    QString currentMonth = currentDate.toString("MM.yyyy");
    QString previousMonth1 = currentDate.addMonths(-1).toString("MM.yyyy");
    QString previousMonth2 = currentDate.addMonths(-2).toString("MM.yyyy");

    double totalExpenses = 0.0;
    double totalIncomes = 0.0;

    for (const Transaction &transaction : qAsConst(m_transactions)) {
        if (transaction.userId != userId) continue;

        if (transaction.status == "Выполнено") {
            QString month = transaction.dateTime.split(' ').first().mid(3, 7); // "MM.yyyy"

            if (transaction.type == "Расход") {
                categoryExpenses[transaction.category] += transaction.amount;
                categoryCounts[transaction.category]++;
                monthlyExpenses[month] += transaction.amount;
                totalExpenses += transaction.amount;
            } else if (transaction.type == "Доход") {
                incomeSources[transaction.category] += transaction.amount;
                monthlyIncomes[month] += transaction.amount;
                totalIncomes += transaction.amount;
            }
        }
    }

    QString report;

    report += "Распределение расходов по категориям:\n";
    for (auto it = categoryExpenses.constBegin(); it != categoryExpenses.constEnd(); ++it) {
        double percentage = (totalExpenses > 0) ? (it.value() / totalExpenses) * 100.0 : 0.0;
        int count = categoryCounts[it.key()];
        QString transactionWord = (count % 10 == 1 && count % 100 != 11) ? "транзакция" :
                                      (count % 10 >= 2 && count % 10 <= 4 && (count % 100 < 10 || count % 100 >= 20)) ? "транзакции" : "транзакций";
        report += QString("%1: %2 %3 (%4 %5, %6\u0025)\n")
                      .arg(it.key())
                      .arg(it.value(), 0, 'f', 2).replace('.', ',')
                      .arg(symbolValute)
                      .arg(count)
                      .arg(transactionWord)
                      .arg(percentage, 0, 'f', 1).replace('.', ',');
    }

    report += "\nТоп-3 категории расходов:\n";
    QList<QPair<QString, double>> sortedExpenses;
    for (auto it = categoryExpenses.constBegin(); it != categoryExpenses.constEnd(); ++it) {
        sortedExpenses.append(qMakePair(it.key(), it.value()));
    }
    std::sort(sortedExpenses.begin(), sortedExpenses.end(), [](const QPair<QString, double> &a, const QPair<QString, double> &b) {
        return a.second > b.second;
    });

    for (int i = 0; i < qMin(3, sortedExpenses.size()); ++i) {
        report += QString("%1. %2: %3 %4\n")
        .arg(i + 1)
            .arg(sortedExpenses[i].first)
            .arg(sortedExpenses[i].second, 0, 'f', 2).replace('.', ',')
            .arg(symbolValute);
    }

    double avgLast3MonthsExpenses = 0.0;
    double avgLast3MonthsIncomes = 0.0;
    double predictedNextMonthExpenses = 0.0;

    QStringList recentMonths = {previousMonth2, previousMonth1, currentMonth};
    for (const QString &month : recentMonths) {
        avgLast3MonthsExpenses += monthlyExpenses.value(month, 0.0);
        avgLast3MonthsIncomes += monthlyIncomes.value(month, 0.0);
    }
    avgLast3MonthsExpenses /= 3.0;
    avgLast3MonthsIncomes /= 3.0;

    predictedNextMonthExpenses = avgLast3MonthsExpenses * 1.05;

    report += "\nТренд расходов и доходов за последние 3 месяца:\n";
    for (const QString &month : recentMonths) {
        report += QString("%1: Расходы: %2 %3, Доходы: %4 %5\n")
                      .arg(month)
                      .arg(monthlyExpenses.value(month, 0.0), 0, 'f', 2).replace('.', ',')
                      .arg(symbolValute)
                      .arg(monthlyIncomes.value(month, 0.0), 0, 'f', 2).replace('.', ',')
                      .arg(symbolValute);
    }
    report += QString("Среднемесячные расходы: %1 %2\n").arg(avgLast3MonthsExpenses, 0, 'f', 2).replace('.', ',').arg(symbolValute);
    report += QString("Среднемесячные доходы: %1 %2\n").arg(avgLast3MonthsIncomes, 0, 'f', 2).replace('.', ',').arg(symbolValute);
    report += QString("Прогноз расходов на следующий месяц: %1 %2\n").arg(predictedNextMonthExpenses, 0, 'f', 2).replace('.', ',').arg(symbolValute);

    report += "\nРедкие категории (использовались только 1 раз):\n";
    for (auto it = categoryCounts.constBegin(); it != categoryCounts.constEnd(); ++it) {
        if (it.value() == 1) {
            report += QString("%1: %2 %3\n")
            .arg(it.key())
                .arg(categoryExpenses[it.key()], 0, 'f', 2).replace('.', ',')
                .arg(symbolValute);
        }
    }

    report += "\nОсновные источники дохода:\n";
    for (auto it = incomeSources.constBegin(); it != incomeSources.constEnd(); ++it) {
        report += QString("%1: %2 %3\n")
        .arg(it.key())
            .arg(it.value(), 0, 'f', 2).replace('.', ',')
            .arg(symbolValute);
    }

    return report;
}

QString TransactionModel::getAdvice(int userId, QString symbolValute) const {
    double totalIncome = 0.0;
    double totalExpenses = 0.0;
    QHash<QString, double> categoryExpenses;

    for (const Transaction &transaction : qAsConst(m_transactions)) {
        if (transaction.userId != userId) continue;

        if (transaction.status == "Выполнено") {
            if (transaction.type == "Доход") {
                totalIncome += transaction.amount;
            } else if (transaction.type == "Расход") {
                totalExpenses += transaction.amount;
                categoryExpenses[transaction.category] += transaction.amount;
            }
        }
    }

    QString advice;

    if (totalIncome == 0) {
        advice = "У вас нет доходов. Это может быть рискованно в долгосрочной перспективе.\nРассмотрите возможность поиска новых источников дохода, например, фриланс, продажа товаров или услуг. Также важно развивать профессиональные навыки, чтобы повысить свою конкурентоспособность на рынке.";
    } else if (totalExpenses > totalIncome) {
        advice = "Ваши расходы превышают доходы. Это может привести к накоплению долгов в будущем.\nСократите расходы, начните с анализа необязательных расходов, таких как развлечения или покупки. Прежде чем делать большую покупку, задайте себе вопрос: действительно ли она необходима?";
    } else {
        double savingsRate = (totalIncome - totalExpenses) / totalIncome * 100;
        if (savingsRate < 5) {
            advice = "Ваш уровень сбережений составляет менее 5%. Это опасно при возникновении непредвиденных ситуаций.\nРекомендуется увеличивать сбережения хотя бы до 10%, чтобы создать финансовую подушку безопасности. Постарайтесь автоматизировать процесс накоплений и откладывать деньги на отдельный счёт.";
        } else if (savingsRate < 15) {
            advice = "Ваш уровень сбережений составляет менее 15%. Вы на правильном пути к финансовой стабильности.\nОднако стоит увеличить этот процент, чтобы быть готовым к непредвиденным обстоятельствам. Начните с создания резервного фонда на 3-6 месяцев.";
        } else if (savingsRate < 30) {
            advice = "Ваш уровень сбережений на хорошем уровне (15-30%).\nТеперь подумайте о долгосрочных инвестициях, таких как недвижимость, пенсионные накопления или диверсифицированный инвестиционный портфель.";
        } else {
            advice = "У вас высокий уровень сбережений (>30%).\nЭто дает вам финансовую свободу для долгосрочных целей и рискованных инвестиций, таких как стартапы или акции. Но не забывайте о безопасности — рассмотрите возможность создания дополнительного резервного фонда.";
        }
    }

    if (!categoryExpenses.isEmpty()) {
        auto maxExpenseIt = std::max_element(categoryExpenses.constBegin(), categoryExpenses.constEnd(),
                                             [](const double &a, const double &b) {
                                                 return a < b;
                                             });

        if (maxExpenseIt != categoryExpenses.constEnd()) {
            QString maxCategory = maxExpenseIt.key();
            double maxAmount = maxExpenseIt.value();
            double maxPercentage = (maxAmount / totalExpenses) * 100;

            QString percentageString = QString::number(maxPercentage, 'f', 1) + "%";

            advice += QString("\n\nВы больше всего тратите на категорию '%1' (%2 %3, %4 от общих расходов).\nПодумайте, можно ли уменьшить расходы в этой области. Возможно, стоит пересмотреть свои привычки или найти более выгодные предложения на рынке.")
                          .arg(maxCategory)
                          .arg(maxAmount, 0, 'f', 2)
                          .arg(symbolValute)
                          .arg(percentageString);
        }

        for (auto it = categoryExpenses.constBegin(); it != categoryExpenses.constEnd(); ++it) {
            QString category = it.key();
            double amount = it.value();
            if (amount / totalExpenses > 0.2) {
                advice += QString("\n\nВы тратите более 20%% от своих расходов на категорию '%1'.\nПопробуйте уменьшить расходы в этой области. Например, можно пересмотреть подписки на сервисы, купить более дешевые товары или услуги, или отказаться от лишних покупок.")
                              .arg(category);
            }
        }
    }

    if (totalIncome > 0 && totalExpenses / totalIncome > 0.5) {
        advice += "\n\nВаши расходы составляют более 50% от доходов.\nРассмотрите возможность поиска дополнительных источников дохода. Это могут быть подработки, фриланс или инвестиции в быстроокупаемые проекты.";
    }

    if (totalIncome > 0 && totalExpenses / totalIncome > 1) {
        advice += "\n\nЕсли ваши расходы регулярно превышают доходы, это может привести к накоплению долгов. Начните вести учет всех своих доходов и расходов. Разработайте бюджет, который позволит вам контролировать свои финансы и избегать долгов.";
    }

    advice += "\n\nВажно составить финансовый план, включающий как краткосрочные, так и долгосрочные цели. Это поможет вам контролировать расходы, ставить приоритеты и достигать финансовых целей.";

    return advice;
}

void TransactionModel::addPlan(const int userId, const QString &type, const QString &name, const QString &amount, const QString &description) {
    QSqlQuery query;
    query.prepare("INSERT INTO plans (user_id, type, name, amount, description) VALUES (:user_id, :type, :name, :amount, :description)");
    query.bindValue(":user_id", userId);
    query.bindValue(":type", type);
    query.bindValue(":name", name);
    query.bindValue(":amount", amount);
    query.bindValue(":description", description);

    if (!query.exec()) {
        qWarning() << "План не добавлен" << query.lastError().text();
    } else {
        emit planAdded();
    }
}

QVariantList TransactionModel::fetchPlans(const int userId, const QString &type) {
    QSqlQuery query(db);
    query.prepare("SELECT user_id, name, amount, description FROM plans WHERE type = :type AND user_id = :user_id");
    query.bindValue(":type", type);
    query.bindValue(":user_id", userId);

    QVariantList plans;

    if (query.exec()) {
        while (query.next()) {
            QVariantMap plan;
            plan["name"] = query.value(1).toString();
            plan["amount"] = query.value(2).toDouble();
            plan["description"] = query.value(3).toString();
            plans.append(plan);
        }
    } else {
        qDebug() << "Failed to fetch plans:" << query.lastError().text();
    }

    return plans;
}

bool TransactionModel::removePlan(const int userId, const QString &planName, const QString &planType) {
    QSqlQuery query;
    query.prepare("DELETE FROM plans WHERE user_id = :user_id AND name = :name AND type = :type");
    query.bindValue(":user_id", userId);
    query.bindValue(":name", planName);
    query.bindValue(":type", planType);

    if (!query.exec()) {
        qWarning() << "Failed to delete plan:" << query.lastError().text();
        return false;
    }
    emit planRemoved();
    return true;
}

bool TransactionModel::addTarget(const int userId, const QString &dateStart, const QString &nameT, const double &amount, const double &saveAmount, const QString &category, const QString &description, const QString &source1, const QString &source2, const QString &source3) {
    Target newTarget;
    newTarget.userId = userId;
    newTarget.dateStart = dateStart;
    newTarget.nameTarget = nameT;
    newTarget.amount = amount;
    newTarget.saved_amount = saveAmount;
    newTarget.category = category;
    newTarget.description = description;
    newTarget.source1 = source1;
    newTarget.source2 = source2;
    newTarget.source3 = source3;

    QSqlQuery query;
    query.prepare("INSERT INTO targets (user_id, date_start, target_name, target_amount, saved_amount, category, description, source_1, source_2, source_3) "
                  "VALUES (:user_id, :date_start, :target_name, :target_amount, :saved_amount, :category, :description, :source_1, :source_2, :source_3)");
    query.bindValue(":user_id", userId);
    query.bindValue(":date_start", newTarget.dateStart);
    query.bindValue(":target_name", newTarget.nameTarget);
    query.bindValue(":target_amount", newTarget.amount);
    query.bindValue(":saved_amount", newTarget.saved_amount);
    query.bindValue(":category", newTarget.category);
    query.bindValue(":description", newTarget.description);
    query.bindValue(":source_1", newTarget.source1);
    query.bindValue(":source_2", newTarget.source2);
    query.bindValue(":source_3", newTarget.source3);

    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении цели:" << query.lastError().text();
        return false;
    }

    newTarget.id = query.lastInsertId().toInt(); // Assign the ID after insert
    emit targetAdded();
    m_targets.append(newTarget);
    return true;
}

QVariantList TransactionModel::fetchTargetsHistory(const int userId) {
    QSqlQuery query(db);
    query.prepare("SELECT target_id, date_start, target_name, target_amount, saved_amount, change_date, category, description, source_1, source_2, source_3 FROM target_history WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    QVariantList targets;
    m_targetsHistory.clear();

    if (query.exec()) {
        while (query.next()) {
            QVariantMap target;
            target["target_id"] = query.value(0).toInt();
            target["date_start"] = query.value(1).toString();
            target["target_name"] = query.value(2).toString();
            target["target_amount"] = query.value(3).toDouble();
            target["saved_amount"] = query.value(4).toDouble();
            target["changeDate"] = query.value(5).toString();
            target["category"] = query.value(6).toString();
            target["description"] = query.value(7).toString();
            target["source_1"] = query.value(8).toString();
            target["source_2"] = query.value(9).toString();
            target["source_3"] = query.value(10).toString();
            targets.append(target);

            TargetHistory newTarget;
            newTarget.userId = userId;
            newTarget.targetId = query.value(0).toInt();
            newTarget.dateStart = query.value(1).toString();
            newTarget.nameTarget = query.value(2).toString();
            newTarget.amount = query.value(3).toDouble();
            newTarget.saved_amount = query.value(4).toDouble();
            newTarget.changeDate = query.value(5).toString();
            newTarget.category = query.value(6).toString();
            newTarget.description = query.value(7).toString();
            newTarget.source1 = query.value(8).toString();
            newTarget.source2 = query.value(9).toString();
            newTarget.source3 = query.value(10).toString();
            m_targetsHistory.append(newTarget);
        }
    } else {
        qDebug() << "Failed to fetch targetsHistory:" << query.lastError().text() << "Query:" << query.lastQuery();
    }

    return targets;
}

QVariantList TransactionModel::fetchTargets(const int userId) {
    QSqlQuery query(db);
    query.prepare("SELECT id, date_start, target_name, target_amount, saved_amount, category, description, source_1, source_2, source_3 FROM targets WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    QVariantList targets;
    m_targets.clear();

    if (query.exec()) {
        while (query.next()) {
            QVariantMap target;
            target["id"] = query.value(0).toInt(); // Fetch ID
            target["date_start"] = query.value(1).toString();
            target["target_name"] = query.value(2).toString();
            target["target_amount"] = query.value(3).toDouble();
            target["saved_amount"] = query.value(4).toDouble();
            target["category"] = query.value(5).toString();
            target["description"] = query.value(6).toString();
            target["source_1"] = query.value(7).toString();
            target["source_2"] = query.value(8).toString();
            target["source_3"] = query.value(9).toString();

            Target newTarget;
            newTarget.id = query.value(0).toInt(); // Set ID
            newTarget.userId = userId;
            newTarget.dateStart = query.value(1).toString();
            newTarget.nameTarget = query.value(2).toString();
            newTarget.amount = query.value(3).toDouble();
            newTarget.saved_amount = query.value(4).toDouble();
            newTarget.category = query.value(5).toString();
            newTarget.description = query.value(6).toString();
            newTarget.source1 = query.value(7).toString();
            newTarget.source2 = query.value(8).toString();
            newTarget.source3 = query.value(9).toString();

            if (newTarget.saved_amount >= newTarget.amount) {
                moveTargetToHistory(newTarget.userId, newTarget.nameTarget);
            } else {
                targets.append(target);
                m_targets.append(newTarget);
            }
        }
    } else {
        qDebug() << "Failed to fetch targets:" << query.lastError().text();
    }

    return targets;
}

bool TransactionModel::editTarget(const int userId, const int currentId, const QString &dateStart, const QString &nameT, const double &amount, const double &saveAmount, const QString &category, const QString &description, const QString &source1, const QString &source2, const QString &source3) {
    if (currentId < 0 || currentId >= m_targets.size()) {
        qDebug() << "Индекс вне диапазона:";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE targets SET date_start = :date_start, target_name = :target_name, target_amount = :target_amount, "
                  "saved_amount = :saved_amount, category = :category, description = :description, "
                  "source_1 = :source_1, source_2 = :source_2, source_3 = :source_3 "
                  "WHERE user_id = :user_id AND id = :target_id");

    const Target &targetToUpdate = m_targets[currentId];
    qDebug() << "Updating target with ID:" << targetToUpdate.id;

    query.bindValue(":user_id", userId);
    query.bindValue(":date_start", dateStart);
    query.bindValue(":target_name", nameT);
    query.bindValue(":target_amount", amount);
    query.bindValue(":saved_amount", saveAmount);
    query.bindValue(":category", category);
    query.bindValue(":description", description);
    query.bindValue(":source_1", source1);
    query.bindValue(":source_2", source2);
    query.bindValue(":source_3", source3);
    query.bindValue(":target_id", targetToUpdate.id); // Use the target's ID here

    if (!query.exec()) {
        qDebug() << "Ошибка при изменении цели:" << query.lastError().text();
        return false;
    }

    int rowsAffected = query.numRowsAffected();
    if (rowsAffected == 0) {
        qDebug() << "Нет затронутых строк. Возможно, цель не найдена или данные не изменились.";
        return false;
    }

    Target updatedTarget = targetToUpdate;
    updatedTarget.dateStart = dateStart;
    updatedTarget.nameTarget = nameT;
    updatedTarget.amount = amount;
    updatedTarget.saved_amount = saveAmount;
    updatedTarget.category = category;
    updatedTarget.description = description;
    updatedTarget.source1 = source1;
    updatedTarget.source2 = source2;
    updatedTarget.source3 = source3;
    m_targets[currentId] = updatedTarget;

    emit targetEdited();
    return true;
}

bool TransactionModel::removeTarget(const int userId, const int index) {
    if (index < 0 || index >= m_targets.size()) {
        qDebug() << "Индекс вне диапазона:";
        return false;
    }

    const Target &targetToRemove = m_targets[index];

    if (!db.transaction()) {
        qDebug() << "Error: Unable to start a database transaction." << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM targets WHERE user_id = :user_id AND id = :target_id");
    query.bindValue(":user_id", userId);
    query.bindValue(":target_id", targetToRemove.id);

    if (!query.exec()) {
        qDebug() << "Ошибка при удалении цели:" << query.lastError().text();
        db.rollback();
        return false;
    }

    m_targets.removeAt(index);
    emit targetRemoved();
    db.commit();
    return true;
}

void TransactionModel::moveTargetToHistory(int userId, const QString& targetName) {
    // Перемещение цели в историю
    QSqlQuery selectQuery;
    selectQuery.prepare("SELECT * FROM targets WHERE user_id = :user_id AND target_name = :target_name");
    selectQuery.bindValue(":user_id", userId);
    selectQuery.bindValue(":target_name", targetName);

    if (selectQuery.exec() && selectQuery.next()) {
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO target_history (user_id, target_id, date_start, target_name, target_amount, saved_amount, change_date, category, description, source_1, source_2, source_3) "
                            "VALUES (:user_id, :target_id, :date_start, :target_name, :target_amount, :saved_amount, datetime('now'), :category, :description, :source_1, :source_2, :source_3)");

        insertQuery.bindValue(":user_id", userId);
        insertQuery.bindValue(":target_id", selectQuery.value("id").toInt());
        insertQuery.bindValue(":date_start", selectQuery.value("date_start").toString());
        insertQuery.bindValue(":target_name", selectQuery.value("target_name").toString());
        insertQuery.bindValue(":target_amount", selectQuery.value("target_amount").toDouble());
        insertQuery.bindValue(":saved_amount", selectQuery.value("saved_amount").toDouble());
        insertQuery.bindValue(":category", selectQuery.value("category").toString());
        insertQuery.bindValue(":description", selectQuery.value("description").toString());
        insertQuery.bindValue(":source_1", selectQuery.value("source_1").toString());
        insertQuery.bindValue(":source_2", selectQuery.value("source_2").toString());
        insertQuery.bindValue(":source_3", selectQuery.value("source_3").toString());

        if (!insertQuery.exec()) {
            qDebug() << "Ошибка при перемещении цели в историю:" << insertQuery.lastError().text();
        }

        // Удаление цели из таблицы targets
        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM targets WHERE user_id = :user_id AND target_name = :target_name");
        deleteQuery.bindValue(":user_id", userId);
        deleteQuery.bindValue(":target_name", targetName);

        if (!deleteQuery.exec()) {
            qDebug() << "Ошибка при удалении цели:" << deleteQuery.lastError().text();
        }
    }
}

QString TransactionModel::reversePhoto(QString selectedFile) {
    if (selectedFile.startsWith("file:///")) {
        selectedFile.remove(0, 7);
        selectedFile.remove(0, 1);
    } else {
        if (!QFileInfo::exists(selectedFile)) {
            qWarning() << "Файл не существует:" << selectedFile;
            return "";
        }
    }

    QFile file(selectedFile);
    if (!file.exists()) {
        qWarning() << "Файл не существует:" << selectedFile;
        return "";
    }

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return "data:image/png;base64," + data.toBase64();
    } else {
        qWarning() << "Не удалось открыть фото:" << selectedFile << "Ошибка:" << file.errorString();
        return "";
    }
}
