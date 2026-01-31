#include <iostream>
#include <string>
using namespace std;

// CLASS 1 - BANK ACCOUNT
class Pankkitili
{
public:
    Pankkitili(string); // constructor
    double getBalance();

    virtual bool deposit(double);
    virtual bool withdraw(double);

protected:
    string omistaja;
    double saldo = 0;
};

// CLASS 1 - BANK ACCOUNT - constructor implementation
Pankkitili::Pankkitili(string owner)
{
    omistaja = owner;
}

// CLASS 1 - BANK ACCOUNT - other implementations

double Pankkitili::getBalance()
{
    return saldo;
}

bool Pankkitili::deposit(double amount)
{
    if (amount <= 0)
    {return false};

    saldo += amount;
    return true;
}

bool Pankkitili::withdraw(double amount)
{
    if(amount <=0)            // negative withdraw not allowed
    {return false};

    if(amount > saldo)     // over saldo limit not allowed
    {return false};

    saldo -= amount;    // Sucess withdraw
    return true;
}







int main()
{
    cout << "Hello World!" << endl;
    return 0;
}
