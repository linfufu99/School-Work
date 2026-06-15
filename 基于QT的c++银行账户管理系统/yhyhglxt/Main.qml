import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: mainWindow
    width: 1200
    height: 800
    visible: true
    title: "银行账户管理系统"

    Component.onCompleted: {
        // 连接SQLite数据库
        if (dbManager.connectDatabase("", "bank_system", "", "")) {
            loadAccounts()
        }
    }

    Connections {
        target: dbManager
        function onErrorOccurred(error) {
            messageDialog.text = error
            messageDialog.open()
        }
        function onOperationSuccess(message) {
            messageDialog.text = message
            messageDialog.open()
            loadAccounts()
        }
    }

    function loadAccounts() {
        var accounts = dbManager.getAllAccounts()
        accountModel.clear()
        for (var i = 0; i < accounts.length; i++) {
            accountModel.append(accounts[i])
        }
    }

    ListModel {
        id: accountModel
    }

    ListModel {
        id: transactionModel
    }

    Dialog {
        id: messageDialog
        title: "提示"
        property alias text: messageText.text
        standardButtons: Dialog.Ok
        Text {
            id: messageText
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        // 标题栏
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#2196F3"
            
            Text {
                anchors.centerIn: parent
                text: "银行账户管理系统"
                font.pixelSize: 24
                font.bold: true
                color: "white"
            }
        }

        // 功能按钮区
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                text: "添加账户"
                onClicked: addAccountDialog.open()
            }

            Button {
                text: "刷新"
                onClicked: loadAccounts()
            }

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "搜索账户号/姓名/身份证..."
            }

            Button {
                text: "搜索"
                onClicked: {
                    if (searchField.text.trim() === "") {
                        loadAccounts()
                    } else {
                        var results = dbManager.searchAccounts(searchField.text)
                        accountModel.clear()
                        for (var i = 0; i < results.length; i++) {
                            accountModel.append(results[i])
                        }
                    }
                }
            }
        }

        // 账户列表
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "#ccc"
            border.width: 1

            ListView {
                id: accountListView
                anchors.fill: parent
                anchors.margins: 1
                clip: true

                header: Rectangle {
                    width: parent.width
                    height: 40
                    color: "#f0f0f0"

                    Row {
                        anchors.fill: parent
                        spacing: 0

                        Repeater {
                            model: ["账户ID", "账户号码", "姓名", "身份证", "电话", "余额", "类型", "状态", "操作"]
                            delegate: Rectangle {
                                width: index === 8 ? 200 : (accountListView.width - 200) / 8
                                height: 40
                                border.color: "#ddd"
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: modelData
                                    font.bold: true
                                }
                            }
                        }
                    }
                }

                model: accountModel
                delegate: Rectangle {
                    width: accountListView.width
                    height: 50
                    color: index % 2 === 0 ? "white" : "#f9f9f9"

                    Row {
                        anchors.fill: parent
                        spacing: 0

                        Text {
                            width: (accountListView.width - 200) / 8
                            height: 50
                            text: model.accountId
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            width: (accountListView.width - 200) / 8
                            height: 50
                            text: model.accountNumber
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            width: (accountListView.width - 200) / 8
                            height: 50
                            text: model.accountName
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            width: (accountListView.width - 200) / 8
                            height: 50
                            text: model.idCard
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            width: (accountListView.width - 200) / 8
                            height: 50
                            text: model.phone
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            width: (accountListView.width - 200) / 8
                            height: 50
                            text: "¥" + model.balance.toFixed(2)
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            color: "#4CAF50"
                            font.bold: true
                        }
                        Text {
                            width: (accountListView.width - 200) / 8
                            height: 50
                            text: model.accountType
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            width: (accountListView.width - 200) / 8
                            height: 50
                            text: model.status
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            color: model.status === "正常" ? "#4CAF50" : "#F44336"
                        }

                        Row {
                            width: 200
                            height: 50
                            spacing: 5
                            leftPadding: 5

                            Button {
                                text: "修改"
                                height: 30
                                anchors.verticalCenter: parent.verticalCenter
                                onClicked: {
                                    editAccountDialog.accountId = model.accountId
                                    editAccountDialog.accountName = model.accountName
                                    editAccountDialog.phone = model.phone
                                    editAccountDialog.status = model.status
                                    editAccountDialog.open()
                                }
                            }

                            Button {
                                text: "删除"
                                height: 30
                                anchors.verticalCenter: parent.verticalCenter
                                onClicked: {
                                    confirmDialog.accountId = model.accountId
                                    confirmDialog.open()
                                }
                            }

                            Button {
                                text: "交易"
                                height: 30
                                anchors.verticalCenter: parent.verticalCenter
                                onClicked: {
                                    transactionDialog.accountId = model.accountId
                                    transactionDialog.accountNumber = model.accountNumber
                                    transactionDialog.accountName = model.accountName
                                    transactionDialog.balance = model.balance
                                    transactionDialog.open()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 添加账户对话框
    Dialog {
        id: addAccountDialog
        title: "添加账户"
        width: 400
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            if (dbManager.addAccount(
                accountNumberField.text,
                accountNameField.text,
                idCardField.text,
                phoneField.text,
                parseFloat(balanceField.text) || 0,
                accountTypeCombo.currentText
            )) {
                accountNumberField.text = ""
                accountNameField.text = ""
                idCardField.text = ""
                phoneField.text = ""
                balanceField.text = "0"
            }
        }

        GridLayout {
            anchors.fill: parent
            columns: 2
            rowSpacing: 10
            columnSpacing: 10

            Label { text: "账户号码:" }
            TextField { id: accountNumberField; Layout.fillWidth: true }

            Label { text: "账户名称:" }
            TextField { id: accountNameField; Layout.fillWidth: true }

            Label { text: "身份证号:" }
            TextField { id: idCardField; Layout.fillWidth: true }

            Label { text: "联系电话:" }
            TextField { id: phoneField; Layout.fillWidth: true }

            Label { text: "初始余额:" }
            TextField { id: balanceField; text: "0"; Layout.fillWidth: true }

            Label { text: "账户类型:" }
            ComboBox {
                id: accountTypeCombo
                Layout.fillWidth: true
                model: ["储蓄账户", "信用账户", "定期账户"]
            }
        }
    }

    // 修改账户对话框
    Dialog {
        id: editAccountDialog
        title: "修改账户"
        width: 400
        standardButtons: Dialog.Ok | Dialog.Cancel

        property int accountId: 0
        property string accountName: ""
        property string phone: ""
        property string status: ""

        onOpened: {
            editNameField.text = accountName
            editPhoneField.text = phone
            // 设置状态下拉框的选中项
            var statusIndex = editStatusCombo.model.indexOf(status)
            if (statusIndex >= 0) {
                editStatusCombo.currentIndex = statusIndex
            }
        }

        onAccepted: {
            dbManager.updateAccount(accountId, editNameField.text, editPhoneField.text, editStatusCombo.currentText)
        }

        GridLayout {
            anchors.fill: parent
            columns: 2
            rowSpacing: 10
            columnSpacing: 10

            Label { text: "账户名称:" }
            TextField { id: editNameField; Layout.fillWidth: true }

            Label { text: "联系电话:" }
            TextField { id: editPhoneField; Layout.fillWidth: true }

            Label { text: "账户状态:" }
            ComboBox {
                id: editStatusCombo
                Layout.fillWidth: true
                model: ["正常", "冻结", "注销"]
            }
        }
    }

    // 删除确认对话框
    Dialog {
        id: confirmDialog
        title: "确认删除"
        property int accountId: 0
        standardButtons: Dialog.Yes | Dialog.No

        Text {
            text: "确定要删除该账户吗？此操作不可恢复！"
        }

        onAccepted: {
            dbManager.deleteAccount(accountId)
        }
    }

    // 交易对话框
    Dialog {
        id: transactionDialog
        title: "账户交易"
        width: 500
        height: 600

        property int accountId: 0
        property string accountNumber: ""
        property string accountName: ""
        property double balance: 0

        onOpened: {
            var transactions = dbManager.getTransactionsByAccountId(accountId)
            transactionModel.clear()
            for (var i = 0; i < transactions.length; i++) {
                transactionModel.append(transactions[i])
            }
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            // 账户信息
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                color: "#E3F2FD"
                radius: 5

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    Text {
                        text: "账户: " + transactionDialog.accountNumber + " - " + transactionDialog.accountName
                        font.pixelSize: 16
                        font.bold: true
                    }
                    Text {
                        text: "余额: ¥" + transactionDialog.balance.toFixed(2)
                        font.pixelSize: 18
                        color: "#4CAF50"
                        font.bold: true
                    }
                }
            }

            // 交易操作
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                ComboBox {
                    id: transactionTypeCombo
                    model: ["存款", "取款", "转账"]
                }

                TextField {
                    id: amountField
                    Layout.fillWidth: true
                    placeholderText: "金额"
                }

                TextField {
                    id: targetAccountField
                    Layout.fillWidth: true
                    placeholderText: "目标账户(转账时填写)"
                    visible: transactionTypeCombo.currentText === "转账"
                }

                Button {
                    text: "执行"
                    onClicked: {
                        var amount = parseFloat(amountField.text)
                        var desc = transactionDescField.text || "无备注"
                        
                        if (transactionTypeCombo.currentText === "存款") {
                            dbManager.deposit(transactionDialog.accountId, amount, desc)
                        } else if (transactionTypeCombo.currentText === "取款") {
                            dbManager.withdraw(transactionDialog.accountId, amount, desc)
                        } else if (transactionTypeCombo.currentText === "转账") {
                            dbManager.transfer(transactionDialog.accountId, targetAccountField.text, amount, desc)
                        }
                        
                        amountField.text = ""
                        targetAccountField.text = ""
                        transactionDescField.text = ""
                        transactionDialog.close()
                    }
                }
            }

            TextField {
                id: transactionDescField
                Layout.fillWidth: true
                placeholderText: "备注说明"
            }

            // 交易记录
            Label {
                text: "交易记录"
                font.bold: true
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: transactionModel

                delegate: Rectangle {
                    width: parent.width
                    height: 60
                    border.color: "#ddd"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10

                        ColumnLayout {
                            Layout.fillWidth: true

                            Text {
                                text: model.transactionType + " - ¥" + model.amount.toFixed(2)
                                font.bold: true
                                color: model.transactionType === "存款" ? "#4CAF50" : "#F44336"
                            }
                            Text {
                                text: model.description
                                font.pixelSize: 12
                                color: "#666"
                            }
                        }

                        ColumnLayout {
                            Text {
                                text: "余额: ¥" + model.balanceAfter.toFixed(2)
                                font.pixelSize: 12
                            }
                            Text {
                                text: model.transactionTime
                                font.pixelSize: 10
                                color: "#999"
                            }
                        }
                    }
                }
            }

            Button {
                text: "关闭"
                Layout.alignment: Qt.AlignRight
                onClicked: transactionDialog.close()
            }
        }
    }
}
