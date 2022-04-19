#ifndef USER_H
#define USER_H

#include <QString>
#include <QVector>

#include "delivery.h"

class person {
protected:
    //用户余额
    int _balance;
    //用户昵称
    QString _userName;
    //用户姓名
    QString _name;
    //用户位置
    QString _location;

public:
    //获取用户余额
    int getBalance() const ;
    //获取用户昵称
    QString getUserName() const ;
    //获取用户位置
    QString getLocation() const ;
    //获取用户姓名
    QString getName() const ;
    //发送快递后扣减余额
    void setBalance(int);
    //充值
    void recharge(int);
};

class user : public person {
private:
    //电话号码
    QString _telephone;

public:
    //默认构造函数
    user();
    //拷贝构造函数
    user(const user& other);
    //有参构造函数，用于注册和读入文本信息时
    user(int balance,QString username,QString name,
            QString location,QString telephone);
    //获得用户电话
    QString getTelephone() const ;
};
#endif // USER_H
