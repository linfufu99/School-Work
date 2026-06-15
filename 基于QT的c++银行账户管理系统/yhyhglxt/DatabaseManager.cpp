#include "DatabaseManager.h"
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    disconnectDatabase();
}

bool DatabaseManager::connectDatabase(const QString &host, const QString &dbName,
                                     const QString &user, const QString &password)
{
    Q_UNUSED(host);
    Q_UNUSED(user);
    Q_UNUSED(password);
    
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName + ".db");

    if (!db.open()) {
        emit errorOccurred("数据库连接失败: " + db.lastError().text());
        return false;
    }

    // 创建表结构
    QSqlQuery query(db);
    
    // 创建账户表
    query.exec("CREATE TABLE IF NOT EXISTS accounts ("
               "account_id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "account_number TEXT UNIQUE NOT NULL, "
               "account_name TEXT NOT NULL, "
               "id_card TEXT NOT NULL, "
               "phone TEXT, "
               "balance REAL DEFAULT 0.00, "
               "account_type TEXT DEFAULT '储蓄账户', "
               "status TEXT DEFAULT '正常', "
               "create_time DATETIME DEFAULT CURRENT_TIMESTAMP, "
               "update_time DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // 创建交易记录表
    query.exec("CREATE TABLE IF NOT EXISTS transactions ("
               "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "account_id INTEGER NOT NULL, "
               "transaction_type TEXT NOT NULL, "
               "amount REAL NOT NULL, "
               "balance_after REAL NOT NULL, "
               "target_account TEXT, "
               "description TEXT, "
               "transaction_time DATETIME DEFAULT CURRENT_TIMESTAMP, "
               "FOREIGN KEY (account_id) REFERENCES accounts(account_id) ON DELETE CASCADE)");

    // 检查是否有数据，没有则插入测试数据
    query.exec("SELECT COUNT(*) FROM accounts");
    if (query.next() && query.value(0).toInt() == 0) {
        query.exec("INSERT INTO accounts (account_number, account_name, id_card, phone, balance, account_type, status) VALUES "
                   "('6222021234567890', '张三', '110101199001011234', '13800138000', 50000.00, '储蓄账户', '正常')");
        query.exec("INSERT INTO accounts (account_number, account_name, id_card, phone, balance, account_type, status) VALUES "
                   "('6222021234567891', '李四', '110101199002021235', '13800138001', 30000.00, '储蓄账户', '正常')");
        query.exec("INSERT INTO accounts (account_number, account_name, id_card, phone, balance, account_type, status) VALUES "
                   "('6222021234567892', '王五', '110101199003031236', '13800138002', 100000.00, '定期账户', '正常')");
        
        query.exec("INSERT INTO transactions (account_id, transaction_type, amount, balance_after, description) VALUES "
                   "(1, '存款', 10000.00, 50000.00, '初始存款')");
        query.exec("INSERT INTO transactions (account_id, transaction_type, amount, balance_after, description) VALUES "
                   "(2, '存款', 30000.00, 30000.00, '初始存款')");
        query.exec("INSERT INTO transactions (account_id, transaction_type, amount, balance_after, description) VALUES "
                   "(3, '存款', 100000.00, 100000.00, '定期存款')");
    }

    emit operationSuccess("数据库连接成功");
    return true;
}

void DatabaseManager::disconnectDatabase()
{
    if (db.isOpen()) {
        db.close();
    }
}

QVariantList DatabaseManager::getAllAccounts()
{
    QVariantList accounts;
    QSqlQuery query(db);
    
    if (!query.exec("SELECT * FROM accounts ORDER BY account_id")) {
        emit errorOccurred("查询失败: " + query.lastError().text());
        return accounts;
    }

    while (query.next()) {
        QVariantMap account;
        account["accountId"] = query.value("account_id").toInt();
        account["accountNumber"] = query.value("account_number").toString();
        account["accountName"] = query.value("account_name").toString();
        account["idCard"] = query.value("id_card").toString();
        account["phone"] = query.value("phone").toString();
        account["balance"] = query.value("balance").toDouble();
        account["accountType"] = query.value("account_type").toString();
        account["status"] = query.value("status").toString();
        account["createTime"] = query.value("create_time").toString();
        accounts.append(account);
    }

    return accounts;
}

QVariantMap DatabaseManager::getAccountById(int accountId)
{
    QVariantMap account;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM accounts WHERE account_id = ?");
    query.addBindValue(accountId);

    if (!query.exec() || !query.next()) {
        emit errorOccurred("查询账户失败");
        return account;
    }

    account["accountId"] = query.value("account_id").toInt();
    account["accountNumber"] = query.value("account_number").toString();
    account["accountName"] = query.value("account_name").toString();
    account["idCard"] = query.value("id_card").toString();
    account["phone"] = query.value("phone").toString();
    account["balance"] = query.value("balance").toDouble();
    account["accountType"] = query.value("account_type").toString();
    account["status"] = query.value("status").toString();
    account["createTime"] = query.value("create_time").toString();

    return account;
}

bool DatabaseManager::addAccount(const QString &accountNumber, const QString &accountName,
                                const QString &idCard, const QString &phone,
                                double balance, const QString &accountType)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO accounts (account_number, account_name, id_card, phone, balance, account_type) "
                 "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(accountNumber);
    query.addBindValue(accountName);
    query.addBindValue(idCard);
    query.addBindValue(phone);
    query.addBindValue(balance);
    query.addBindValue(accountType);

    if (!query.exec()) {
        emit errorOccurred("添加账户失败: " + query.lastError().text());
        return false;
    }

    emit operationSuccess("账户添加成功");
    return true;
}

bool DatabaseManager::updateAccount(int accountId, const QString &accountName,
                                   const QString &phone, const QString &status)
{
    QSqlQuery query(db);
    query.prepare("UPDATE accounts SET account_name = ?, phone = ?, status = ? WHERE account_id = ?");
    query.addBindValue(accountName);
    query.addBindValue(phone);
    query.addBindValue(status);
    query.addBindValue(accountId);

    if (!query.exec()) {
        emit errorOccurred("更新账户失败: " + query.lastError().text());
        return false;
    }

    emit operationSuccess("账户更新成功");
    return true;
}

bool DatabaseManager::deleteAccount(int accountId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM accounts WHERE account_id = ?");
    query.addBindValue(accountId);

    if (!query.exec()) {
        emit errorOccurred("删除账户失败: " + query.lastError().text());
        return false;
    }

    emit operationSuccess("账户删除成功");
    return true;
}

QVariantList DatabaseManager::searchAccounts(const QString &keyword)
{
    QVariantList accounts;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM accounts WHERE account_number LIKE ? OR account_name LIKE ? OR id_card LIKE ?");
    QString searchPattern = "%" + keyword + "%";
    query.addBindValue(searchPattern);
    query.addBindValue(searchPattern);
    query.addBindValue(searchPattern);

    if (!query.exec()) {
        emit errorOccurred("搜索失败: " + query.lastError().text());
        return accounts;
    }

    while (query.next()) {
        QVariantMap account;
        account["accountId"] = query.value("account_id").toInt();
        account["accountNumber"] = query.value("account_number").toString();
        account["accountName"] = query.value("account_name").toString();
        account["idCard"] = query.value("id_card").toString();
        account["phone"] = query.value("phone").toString();
        account["balance"] = query.value("balance").toDouble();
        account["accountType"] = query.value("account_type").toString();
        account["status"] = query.value("status").toString();
        accounts.append(account);
    }

    return accounts;
}

QVariantList DatabaseManager::getTransactionsByAccountId(int accountId)
{
    QVariantList transactions;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM transactions WHERE account_id = ? ORDER BY transaction_time DESC");
    query.addBindValue(accountId);

    if (!query.exec()) {
        emit errorOccurred("查询交易记录失败: " + query.lastError().text());
        return transactions;
    }

    while (query.next()) {
        QVariantMap transaction;
        transaction["transactionId"] = query.value("transaction_id").toInt();
        transaction["transactionType"] = query.value("transaction_type").toString();
        transaction["amount"] = query.value("amount").toDouble();
        transaction["balanceAfter"] = query.value("balance_after").toDouble();
        transaction["targetAccount"] = query.value("target_account").toString();
        transaction["description"] = query.value("description").toString();
        transaction["transactionTime"] = query.value("transaction_time").toString();
        transactions.append(transaction);
    }

    return transactions;
}

bool DatabaseManager::deposit(int accountId, double amount, const QString &description)
{
    if (amount <= 0) {
        emit errorOccurred("存款金额必须大于0");
        return false;
    }

    db.transaction();

    double currentBalance = getAccountBalance(accountId);
    double newBalance = currentBalance + amount;

    if (!updateAccountBalance(accountId, newBalance)) {
        db.rollback();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO transactions (account_id, transaction_type, amount, balance_after, description) "
                 "VALUES (?, '存款', ?, ?, ?)");
    query.addBindValue(accountId);
    query.addBindValue(amount);
    query.addBindValue(newBalance);
    query.addBindValue(description);

    if (!query.exec()) {
        db.rollback();
        emit errorOccurred("存款失败: " + query.lastError().text());
        return false;
    }

    db.commit();
    emit operationSuccess("存款成功");
    return true;
}

bool DatabaseManager::withdraw(int accountId, double amount, const QString &description)
{
    if (amount <= 0) {
        emit errorOccurred("取款金额必须大于0");
        return false;
    }

    db.transaction();

    double currentBalance = getAccountBalance(accountId);
    if (currentBalance < amount) {
        db.rollback();
        emit errorOccurred("余额不足");
        return false;
    }

    double newBalance = currentBalance - amount;

    if (!updateAccountBalance(accountId, newBalance)) {
        db.rollback();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO transactions (account_id, transaction_type, amount, balance_after, description) "
                 "VALUES (?, '取款', ?, ?, ?)");
    query.addBindValue(accountId);
    query.addBindValue(amount);
    query.addBindValue(newBalance);
    query.addBindValue(description);

    if (!query.exec()) {
        db.rollback();
        emit errorOccurred("取款失败: " + query.lastError().text());
        return false;
    }

    db.commit();
    emit operationSuccess("取款成功");
    return true;
}

bool DatabaseManager::transfer(int fromAccountId, const QString &toAccountNumber,
                              double amount, const QString &description)
{
    if (amount <= 0) {
        emit errorOccurred("转账金额必须大于0");
        return false;
    }

    db.transaction();

    // 获取目标账户ID
    QSqlQuery query(db);
    query.prepare("SELECT account_id FROM accounts WHERE account_number = ?");
    query.addBindValue(toAccountNumber);
    
    if (!query.exec() || !query.next()) {
        db.rollback();
        emit errorOccurred("目标账户不存在");
        return false;
    }
    
    int toAccountId = query.value(0).toInt();

    // 检查余额
    double fromBalance = getAccountBalance(fromAccountId);
    if (fromBalance < amount) {
        db.rollback();
        emit errorOccurred("余额不足");
        return false;
    }

    // 扣款
    double newFromBalance = fromBalance - amount;
    if (!updateAccountBalance(fromAccountId, newFromBalance)) {
        db.rollback();
        return false;
    }

    // 入账
    double toBalance = getAccountBalance(toAccountId);
    double newToBalance = toBalance + amount;
    if (!updateAccountBalance(toAccountId, newToBalance)) {
        db.rollback();
        return false;
    }

    // 记录转出交易
    query.prepare("INSERT INTO transactions (account_id, transaction_type, amount, balance_after, target_account, description) "
                 "VALUES (?, '转账', ?, ?, ?, ?)");
    query.addBindValue(fromAccountId);
    query.addBindValue(amount);
    query.addBindValue(newFromBalance);
    query.addBindValue(toAccountNumber);
    query.addBindValue(description);

    if (!query.exec()) {
        db.rollback();
        emit errorOccurred("转账失败: " + query.lastError().text());
        return false;
    }

    db.commit();
    emit operationSuccess("转账成功");
    return true;
}

double DatabaseManager::getAccountBalance(int accountId)
{
    QSqlQuery query(db);
    query.prepare("SELECT balance FROM accounts WHERE account_id = ?");
    query.addBindValue(accountId);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

bool DatabaseManager::updateAccountBalance(int accountId, double newBalance)
{
    QSqlQuery query(db);
    query.prepare("UPDATE accounts SET balance = ? WHERE account_id = ?");
    query.addBindValue(newBalance);
    query.addBindValue(accountId);

    if (!query.exec()) {
        emit errorOccurred("更新余额失败: " + query.lastError().text());
        return false;
    }

    return true;
}
