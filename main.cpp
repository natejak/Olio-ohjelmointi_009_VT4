#include <iostream>
#include <string>
using namespace std;

// ======================================================
// CLASS 1 - BANK ACCOUNT (DEBIT ACCOUNT)
// ======================================================
class Pankkitili
{
public:
    Pankkitili(string);
    double getBalance() const;

    virtual bool deposit(double);
    virtual bool withdraw(double);

protected:
    string omistaja;   // Account owner
    double saldo = 0;  // Account balance
};

Pankkitili::Pankkitili(string owner)
{
    omistaja = owner;
}

double Pankkitili::getBalance() const
{
    return saldo;
}

// Deposit money to debit account
bool Pankkitili::deposit(double amount)
{
    if (amount <= 0)
    {
        cout << "[ERROR] Deposit failed for " << omistaja
             << " | Invalid amount: " << amount << " EUR" << endl;
        return false;
    }

    saldo += amount;
    cout << "[DEPOSIT] " << omistaja << " deposited "
         << amount << " EUR to Debit Account | New balance: " << saldo << " EUR" << endl;
    return true;
}

// Withdraw money from debit account
bool Pankkitili::withdraw(double amount)
{
    if (amount <= 0)
    {
        cout << "[ERROR] Withdraw failed for " << omistaja
             << " | Invalid amount: " << amount << " EUR" << endl;
        return false;
    }

    if (amount > saldo)
    {
        cout << "[ERROR] Withdraw failed for " << omistaja
             << " | Not enough funds. Requested: " << amount
             << " EUR, Available: " << saldo << " EUR" << endl;
        return false;
    }

    saldo -= amount;
    cout << "[WITHDRAW] " << omistaja << " withdrew "
         << amount << " EUR from Debit Account | New balance: " << saldo << " EUR" << endl;
    return true;
}

// ======================================================
// CLASS 2 - CREDIT ACCOUNT
// ======================================================
class Luottotili : public Pankkitili
{
public:
    Luottotili(string, double);

    bool deposit(double) override;  // Credit repayment (balance must never be positive)
    bool withdraw(double) override; // Credit withdrawal (balance allowed down to -creditLimit)

protected:
    double luottoRaja = 0; // Credit limit
};

Luottotili::Luottotili(string owner, double limit)
    : Pankkitili(owner)
{
    luottoRaja = limit;
    cout << "[INFO] Assigned new Credit Account for: " << omistaja
         << " | Credit limit: " << luottoRaja << " EUR" << endl;
}

// Withdraw money using credit
bool Luottotili::withdraw(double amount)
{
    if (amount <= 0)
    {
        cout << "[ERROR] Credit withdrawal failed for " << omistaja
             << " | Invalid amount: " << amount << " EUR" << endl;
        return false;
    }

    // New balance after withdrawal would be: saldo - amount
    double newSaldo = saldo - amount;

    // Credit limit check: newSaldo must not be less than -luottoRaja
    if (newSaldo < -luottoRaja)
    {
        cout << "[ERROR] Credit withdrawal failed for " << omistaja
             << " | Credit limit exceeded. Requested: " << amount
             << " EUR, Current credit balance: " << saldo
             << " EUR, Limit: " << luottoRaja << " EUR" << endl;
        cout << "        Lowest allowed balance is: " << -luottoRaja
             << " EUR, attempted balance would be: " << newSaldo << " EUR" << endl;
        return false;
    }

    saldo = newSaldo;

    cout << "[CREDIT WITHDRAW] " << omistaja
         << " used credit: " << amount
         << " EUR | Credit balance: " << saldo << " EUR"
         << " | Credit left: " << (luottoRaja + saldo) << " EUR" << endl;
    return true;
}

// Pay back credit (saldo must not become positive)
bool Luottotili::deposit(double amount)
{
    if (amount <= 0)
    {
        cout << "[ERROR] Credit payment failed for " << omistaja
             << " | Invalid amount: " << amount << " EUR" << endl;
        return false;
    }

    double before = saldo;
    saldo += amount;

    // Credit balance must not exceed zero
    if (saldo > 0)
    {
        cout << "[WARN] Overpayment detected for " << omistaja
             << " | Payment: " << amount
             << " EUR, balance before payment: " << before
             << " EUR. Capping credit balance to 0 EUR." << endl;
        saldo = 0;
    }

    cout << "[CREDIT PAYMENT] " << omistaja
         << " paid back " << amount
         << " EUR | Credit balance: " << saldo << " EUR"
         << " | Credit left: " << (luottoRaja + saldo) << " EUR" << endl;
    return true;
}

// ======================================================
// CLASS 3 - CUSTOMER
// ======================================================
class Asiakas
{
public:
    Asiakas(string, double);

    string getNimi() const;
    void showSaldo() const;

    bool talletus(double);
    bool nosto(double);

    bool luotonMaksu(double);
    bool luotonNosto(double);

    bool tiliSiirto(double, Asiakas&);

private:
    string nimi;
    Pankkitili kayttotili;
    Luottotili luottotili;
};

Asiakas::Asiakas(string owner, double limit)
    : nimi(owner),
    kayttotili(owner),
    luottotili(owner, limit)
{
    cout << "[INFO] New customer created | Name: " << nimi << endl;
    cout << "[INFO] Assigned new Debit Account for: " << nimi << endl;
}

string Asiakas::getNimi() const
{
    return nimi;
}

void Asiakas::showSaldo() const
{
    cout << "*************************" << endl;
    cout << "Name of Customer: " << getNimi() << endl;
    cout << "Deposit account SALDO: " << kayttotili.getBalance() << " EUR" << endl;
    cout << "Credit account SALDO: " << luottotili.getBalance() << " EUR" << endl;
    cout << "*************************" << endl;
}

bool Asiakas::talletus(double amount)
{
    return kayttotili.deposit(amount);
}

bool Asiakas::nosto(double amount)
{
    return kayttotili.withdraw(amount);
}

bool Asiakas::luotonMaksu(double amount)
{
    return luottotili.deposit(amount);
}

bool Asiakas::luotonNosto(double amount)
{
    return luottotili.withdraw(amount);
}

// Transfer money between customers (debit accounts only)
bool Asiakas::tiliSiirto(double amount, Asiakas &receiver)
{
    if (amount <= 0)
    {
        cout << "[ERROR] Transfer failed from " << nimi << " to " << receiver.getNimi()
        << " | Invalid amount: " << amount << " EUR" << endl;
        return false;
    }

    cout << "[TRANSFER REQUEST] " << nimi << " -> " << receiver.getNimi()
         << " | Amount: " << amount << " EUR" << endl;

    // Withdraw from sender's debit account
    if (!nosto(amount))
    {
        cout << "[ERROR] Transfer failed from " << nimi << " to " << receiver.getNimi()
        << " | Reason: sender has insufficient funds." << endl;
        return false;
    }

    // Deposit to receiver's debit account
    if (!receiver.talletus(amount))
    {
        cout << "[ERROR] Transfer failed from " << nimi << " to " << receiver.getNimi()
        << " | Reason: receiver deposit failed." << endl;

        return false;
    }

    cout << "[TRANSFER] " << nimi << " transferred "
         << amount << " EUR to " << receiver.getNimi() << endl;
    return true;
}

// ======================================================
// MAIN - TEST SCENARIO
// ======================================================
int main()
{
    Asiakas a("Antti", 500);
    Asiakas b("Petri", 300);

    // Normal successful operations
    a.talletus(200);
    a.tiliSiirto(100, b);

    a.luotonNosto(150);
    a.luotonMaksu(50);

    // test error cases:
    a.nosto(1000);              // too much debit withdrawal
    a.tiliSiirto(1000, b);      // too big transfer
    a.luotonNosto(10000);       // credit limit exceeded
    a.talletus(-5);             // invalid deposit
    a.luotonMaksu(-10);         // invalid credit payment

    cout << "\n=== FINAL BALANCES ===\n\n";

    a.showSaldo();

    b.showSaldo();

    return 0;
}
