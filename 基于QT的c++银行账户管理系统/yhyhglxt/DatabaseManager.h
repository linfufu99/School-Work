#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantList>
#include <QVariantMap>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    Q_INVOKABLE bool connectDatabase(const QString &host, const QString &dbName,
                                     const QString &user, const QString &password);
    Q_INVOKABLE void disconnectDatabase();
    
    // 账户管理
    Q_INVOKABLE QVariantList getAllAccounts();
    Q_INVOKABLE QVariantMap getAccountById(int accountId);
    Q_INVOKABLE bool addAccount(const QString &accountNumber, const QString &accountName,
                               const QString &idCard, const QString &phone,
                               double balance, const QString &accountType);
    Q_INVOKABLE bool updateAccount(int accountId, const QString &accountName,
                                   const QString &phone, const QString &status);
    Q_INVOKABLE bool deleteAccount(int accountId);
    Q_INVOKABLE QVariantList searchAccounts(const QString &keyword);
    
    // 交易管理
    Q_INVOKABLE QVariantList getTransactionsByAccountId(int accountId);
    Q_INVOKABLE bool deposit(int accountId, double amount, const QString &description);
    Q_INVOKABLE bool withdraw(int accountId, double amount, const QString &description);
    Q_INVOKABLE bool transfer(int fromAccountId, const QString &toAccountNumber,
                             double amount, const QString &description);

signals:
    void errorOccurred(const QString &error);
    void operationSuccess(const QString &message);

private:
    QSqlDatabase db;
    bool executeQuery(QSqlQuery &query);
    double getAccountBalance(int accountId);
    bool updateAccountBalance(int accountId, double newBalance);
};

#endif // DATABASEMANAGER_H
