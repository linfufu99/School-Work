-- SQLite版本的银行账户管理系统数据库
-- SQLite会自动创建，此文件仅供参考

-- 1. 账户信息表
CREATE TABLE IF NOT EXISTS accounts (
    account_id INTEGER PRIMARY KEY AUTOINCREMENT,
    account_number TEXT UNIQUE NOT NULL,
    account_name TEXT NOT NULL,
    id_card TEXT NOT NULL,
    phone TEXT,
    balance REAL DEFAULT 0.00,
    account_type TEXT DEFAULT '储蓄账户',
    status TEXT DEFAULT '正常',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    update_time DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 2. 交易记录表
CREATE TABLE IF NOT EXISTS transactions (
    transaction_id INTEGER PRIMARY KEY AUTOINCREMENT,
    account_id INTEGER NOT NULL,
    transaction_type TEXT NOT NULL,
    amount REAL NOT NULL,
    balance_after REAL NOT NULL,
    target_account TEXT,
    description TEXT,
    transaction_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (account_id) REFERENCES accounts(account_id) ON DELETE CASCADE
);

-- 插入测试数据
INSERT INTO accounts (account_number, account_name, id_card, phone, balance, account_type, status) VALUES
('6222021234567890', '张三', '110101199001011234', '13800138000', 50000.00, '储蓄账户', '正常'),
('6222021234567891', '李四', '110101199002021235', '13800138001', 30000.00, '储蓄账户', '正常'),
('6222021234567892', '王五', '110101199003031236', '13800138002', 100000.00, '定期账户', '正常');

INSERT INTO transactions (account_id, transaction_type, amount, balance_after, description) VALUES
(1, '存款', 10000.00, 50000.00, '初始存款'),
(2, '存款', 30000.00, 30000.00, '初始存款'),
(3, '存款', 100000.00, 100000.00, '定期存款');
