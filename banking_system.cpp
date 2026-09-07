
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

const string ACCOUNTS_FILE = "accounts.txt";
const string TXN_FILE = "transactions.txt";
const string ADMIN_PASSWORD = "admin123";

int getIntInput(const string &prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) { cin.ignore(numeric_limits<streamsize>::max(), '\n'); return val; }
        cout << "Invalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}
double getDoubleInput(const string &prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val) { cin.ignore(numeric_limits<streamsize>::max(), '\n'); return val; }
        cout << "Invalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}
string currentTimestamp() {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

class Account {
public:
    long accountNumber;
    string holderName;
    string accountType; 
    double balance;
    bool blocked = false;

    string toFileLine() const {
        stringstream ss;
        ss << accountNumber << "|" << holderName << "|" << accountType << "|"
           << pin << "|" << balance << "|" << (blocked ? 1 : 0);
        return ss.str();
    }
    static Account fromFileLine(const string &line) {
        Account a;
        stringstream ss(line);
        string tok;
        getline(ss, tok, '|'); a.accountNumber = stol(tok);
        getline(ss, a.holderName, '|');
        getline(ss, a.accountType, '|');
        getline(ss, a.pin, '|');
        getline(ss, tok, '|'); a.balance = stod(tok);
        getline(ss, tok, '|'); a.blocked = (tok == "1");
        return a;
    }
};

struct Transaction {
    long accountNumber;
    string type; 
    double amount;
    string timestamp;
    string note;

    string toFileLine() const {
        stringstream ss;
        ss << accountNumber << "|" << type << "|" << amount << "|" << timestamp << "|" << note;
        return ss.str();
    }
    static Transaction fromFileLine(const string &line) {
        Transaction t;
        stringstream ss(line);
        string tok;
        getline(ss, tok, '|'); t.accountNumber = stol(tok);
        getline(ss, t.type, '|');
        getline(ss, tok, '|'); t.amount = stod(tok);
        getline(ss, t.timestamp, '|');
        getline(ss, t.note, '|');
        return t;
    }
};

class BankingSystem {
    vector<Account> accounts;
    vector<Transaction> transactions;
    Account *loggedIn = nullptr;

public:
    BankingSystem() { loadAccounts(); loadTransactions(); }

    void loadAccounts() {
        accounts.clear();
        ifstream fin(ACCOUNTS_FILE);
        string line;
        while (getline(fin, line)) if (!line.empty()) accounts.push_back(Account::fromFileLine(line));
    }
    void saveAccounts() {
        ofstream fout(ACCOUNTS_FILE, ios::trunc);
        for (auto &a : accounts) fout << a.toFileLine() << "\n";
    }
    void loadTransactions() {
        transactions.clear();
        ifstream fin(TXN_FILE);
        string line;
        while (getline(fin, line)) if (!line.empty()) transactions.push_back(Transaction::fromFileLine(line));
    }
    void logTransaction(long accNum, const string &type, double amount, const string &note) {
        Transaction t{accNum, type, amount, currentTimestamp(), note};
        transactions.push_back(t);
        ofstream fout(TXN_FILE, ios::app);
        fout << t.toFileLine() << "\n";
    }

    Account* findAccount(long accNum) {
        for (auto &a : accounts) if (a.accountNumber == accNum) return &a;
        return nullptr;
    }

    long generateAccountNumber() {
        long maxNum = 100000;
        for (auto &a : accounts) maxNum = max(maxNum, a.accountNumber);
        return maxNum + 1;
    }

    void createAccount() {
        Account a;
        a.accountNumber = generateAccountNumber();
        cout << "\n--- Create Bank Account ---\n";
        cout << "Account Holder Name: "; getline(cin, a.holderName);
        cout << "Account Type (Savings/Current): "; getline(cin, a.accountType);
        cout << "Set a 4-digit PIN: "; getline(cin, a.pin);
        a.balance = getDoubleInput("Initial deposit amount: Rs ");
        if (a.balance < 0) { cout << "Initial deposit cannot be negative.\n"; return; }
        accounts.push_back(a);
        saveAccounts();
        if (a.balance > 0) logTransaction(a.accountNumber, "Deposit", a.balance, "Initial deposit");
        cout << "Account created successfully! Your account number is: " << a.accountNumber << "\n";
    }

    void login() {
        long accNum = getIntInput("Enter Account Number: ");
        string pin;
        cout << "Enter PIN: "; getline(cin, pin);
        Account *a = findAccount(accNum);
        if (!a) { cout << "Account not found.\n"; return; }
        if (a->blocked) { cout << "This account is blocked. Contact admin.\n"; return; }
        if (a->pin != pin) { cout << "Incorrect PIN.\n"; return; }
        loggedIn = a;
        cout << "Login successful. Welcome, " << a->holderName << "!\n";
    }

    void logout() {
        if (loggedIn) { cout << "Logged out. Goodbye, " << loggedIn->holderName << "!\n"; loggedIn = nullptr; }
        else cout << "No user logged in.\n";
    }

    void viewDetails() {
        if (!loggedIn) { cout << "Please login first.\n"; return; }
        cout << fixed << setprecision(2);
        cout << "\n--- Account Details ---\n";
        cout << "Account Number: " << loggedIn->accountNumber << "\nHolder Name: " << loggedIn->holderName
             << "\nAccount Type: " << loggedIn->accountType << "\nBalance: Rs " << loggedIn->balance << "\n";
    }

    void deposit() {
        if (!loggedIn) { cout << "Please login first.\n"; return; }
        double amt = getDoubleInput("Enter amount to deposit: Rs ");
        if (amt <= 0) { cout << "Invalid amount.\n"; return; }
        loggedIn->balance += amt;
        saveAccounts();
        logTransaction(loggedIn->accountNumber, "Deposit", amt, "Cash deposit");
        cout << fixed << setprecision(2);
        cout << "Deposit successful. New balance: Rs " << loggedIn->balance << "\n";
    }

    void withdraw() {
        if (!loggedIn) { cout << "Please login first.\n"; return; }
        double amt = getDoubleInput("Enter amount to withdraw: Rs ");
        if (amt <= 0) { cout << "Invalid amount.\n"; return; }
        if (amt > loggedIn->balance) { cout << "Insufficient balance.\n"; return; }
        loggedIn->balance -= amt;
        saveAccounts();
        logTransaction(loggedIn->accountNumber, "Withdrawal", amt, "Cash withdrawal");
        cout << fixed << setprecision(2);
        cout << "Withdrawal successful. New balance: Rs " << loggedIn->balance << "\n";
    }

    void transfer() {
        if (!loggedIn) { cout << "Please login first.\n"; return; }
        long targetNum = getIntInput("Enter recipient account number: ");
        Account *target = findAccount(targetNum);
        if (!target) { cout << "Recipient account not found.\n"; return; }
        if (target->accountNumber == loggedIn->accountNumber) { cout << "Cannot transfer to your own account.\n"; return; }
        double amt = getDoubleInput("Enter amount to transfer: Rs ");
        if (amt <= 0) { cout << "Invalid amount.\n"; return; }
        if (amt > loggedIn->balance) { cout << "Insufficient balance.\n"; return; }
        loggedIn->balance -= amt;
        target->balance += amt;
        saveAccounts();
        logTransaction(loggedIn->accountNumber, "Transfer-Out", amt, "To A/C " + to_string(target->accountNumber));
        logTransaction(target->accountNumber, "Transfer-In", amt, "From A/C " + to_string(loggedIn->accountNumber));
        cout << fixed << setprecision(2);
        cout << "Transfer successful. New balance: Rs " << loggedIn->balance << "\n";
    }

    void viewTransactionHistory() {
        if (!loggedIn) { cout << "Please login first.\n"; return; }
        cout << fixed << setprecision(2);
        cout << "\n--- Transaction History ---\n";
        bool found = false;
        for (auto &t : transactions) {
            if (t.accountNumber == loggedIn->accountNumber) {
                found = true;
                cout << "[" << t.timestamp << "] " << t.type << " Rs " << t.amount << " | " << t.note << "\n";
            }
        }
        if (!found) cout << "No transactions found.\n";
    }

    
    bool adminLogin() {
        string pass;
        cout << "Enter admin password: "; getline(cin, pass);
        return pass == ADMIN_PASSWORD;
    }

    void adminViewAllAccounts() {
        cout << fixed << setprecision(2);
        cout << "\n--- All Customer Accounts ---\n";
        for (auto &a : accounts) {
            cout << "A/C: " << a.accountNumber << " | " << a.holderName << " | " << a.accountType
                 << " | Balance: Rs " << a.balance << " | " << (a.blocked ? "BLOCKED" : "Active") << "\n";
        }
        if (accounts.empty()) cout << "No accounts found.\n";
    }

    void adminMonitorTransactions() {
        cout << fixed << setprecision(2);
        cout << "\n--- All Transactions ---\n";
        for (auto &t : transactions)
            cout << "[" << t.timestamp << "] A/C " << t.accountNumber << " | " << t.type
                 << " Rs " << t.amount << " | " << t.note << "\n";
        if (transactions.empty()) cout << "No transactions recorded.\n";
    }

    void adminBlockUnblockAccount() {
        long accNum = getIntInput("Enter account number to block/unblock: ");
        Account *a = findAccount(accNum);
        if (!a) { cout << "Account not found.\n"; return; }
        a->blocked = !a->blocked;
        saveAccounts();
        cout << "Account is now " << (a->blocked ? "BLOCKED" : "ACTIVE") << ".\n";
    }

    void adminDeleteAccount() {
        long accNum = getIntInput("Enter account number to delete: ");
        auto it = remove_if(accounts.begin(), accounts.end(),
                             [&](const Account &a) { return a.accountNumber == accNum; });
        if (it == accounts.end()) { cout << "Account not found.\n"; return; }
        accounts.erase(it, accounts.end());
        saveAccounts();
        cout << "Account deleted successfully.\n";
    }

    void adminMenu() {
        if (!adminLogin()) { cout << "Incorrect admin password.\n"; return; }
        int choice;
        do {
            cout << "\n===== ADMIN PANEL =====\n";
            cout << "1. View All Accounts\n2. Monitor All Transactions\n"
                 << "3. Block/Unblock Account\n4. Delete Account\n0. Back\n";
            choice = getIntInput("Enter choice: ");
            switch (choice) {
                case 1: adminViewAllAccounts(); break;
                case 2: adminMonitorTransactions(); break;
                case 3: adminBlockUnblockAccount(); break;
                case 4: adminDeleteAccount(); break;
                case 0: break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 0);
    }

    void mainMenu() {
        int choice;
        do {
            cout << "\n===== BANKING MANAGEMENT SYSTEM =====\n";
            cout << "Logged in as: " << (loggedIn ? loggedIn->holderName : "Guest") << "\n";
            cout << "1. Create Account\n2. Login\n3. Logout\n4. View Account Details\n"
                 << "5. Deposit\n6. Withdraw\n7. Transfer\n8. Transaction History\n"
                 << "9. Admin Panel\n0. Exit\n";
            choice = getIntInput("Enter choice: ");
            switch (choice) {
                case 1: createAccount(); break;
                case 2: login(); break;
                case 3: logout(); break;
                case 4: viewDetails(); break;
                case 5: deposit(); break;
                case 6: withdraw(); break;
                case 7: transfer(); break;
                case 8: viewTransactionHistory(); break;
                case 9: adminMenu(); break;
                case 0: cout << "Thank you for banking with us!\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    BankingSystem bank;
    bank.mainMenu();
    return 0;
}
