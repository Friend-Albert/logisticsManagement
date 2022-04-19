#include "user.h"

int person::getBalance() const {return _balance;}

QString person::getUserName() const { return _userName; }

QString person::getLocation() const {return _location; }

QString person::getName() const {return _name; }

void person::setBalance(int price)
{
    _balance -= price;
}

void person::recharge(int money)
{
    _balance += money;
}

user::user(){
    _balance = 0;
}

user::user(const user& other){
    _balance = other._balance;
    _userName = other._userName;
    _name = other._name;
    _location = other._location;
    _telephone = other._telephone;
}

user::user(int balance,QString username,QString name,
    QString location,QString telephone){
    _balance = balance;
    _userName = username;
    _name = name;
    _location = location;
    _telephone = telephone;
}
QString user::getTelephone() const {
    return _telephone;
}
