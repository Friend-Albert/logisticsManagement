#ifndef DELIVERY_H
#define DELIVERY_H

#include <QString>
#include "basic.h"

//所有快递种类的基类
class delivery {
public:
    enum type{
        toBeCollected = 0x01,
        toBeSigned = 0x02,
        hasSigned = 0x04
    };
protected:
    //用于区别快递的唯一ID
    int _id;
    //件数
    int _number;
    //快递寄件人的USERID以及用户名
    QString _sender;
    //快递收件人的USERID以及用户名
    QString _addressee;
    //快递状态
    int _status;
    //寄件时间
    mytime _sendTime;
    //收件时间
    mytime _recvTime;
    //快递描述
    QString _description;
    //快递员名称
    QString _postman;
    //快递信息是否发生了更改，需要更新文件中的信息
    bool needRewrite = false;

public:
    //虚析构函数
    virtual ~delivery(){};
    //设置快递签收
    void setSigned();
    //计算价格的纯虚函数
    virtual int getPrice() const = 0;
    //返回快递类型的纯虚函数
    virtual int getType() const = 0;
    //获得快递ID
    int getid() const ;
    //获得寄件人信息
    QString getSender() const ;
    //获得收件人信息
    QString getAddressee() const ;
    //获得快递描述
    QString getDescription() const ;
    //获得快递状态
    int getStatus() const ;
    //更改快递状态
    void setStatus();
    //获得快递寄件时间
    mytime getSendTime() const ;
    //获得快递收件时间
    mytime getRecvTime() const ;
    //获取是否需要重写
    bool getNeedRewrite() const;
    //设置需要重写
    void setNeedRewrite();
    //获得快递员名称
    const QString &getPostman() const;
    //设置快递员
    void setPostman(const QString &newPostman);
    //获得件数
    int getNumber() const;
};

//阶段一唯一的一种快递
class single : public delivery {
public:
    //默认构造
    single();
    //有参构造函数，用于用户寄出新快递时
    single(int, QString, QString,
           mytime,QString);
    //有参构造函数，用于系统读入原有快递记录时
    single(int, QString, QString,
           mytime, mytime, QString,int);
    //父类计算快递价格函数的重写
    virtual int getPrice() const;
    //返回快递类型
    virtual int getType() const;
};

class book : public delivery {
public:
    book(int, int, int, QString, QString,QString,mytime, QString, mytime = mytime());
    virtual ~book();
    virtual int getType() const;
    virtual int getPrice() const;
};

class fragile : public delivery {
public:
    fragile(int, int, int, QString, QString,QString,mytime, QString, mytime = mytime());
    virtual ~fragile();
    virtual int getType() const;
    virtual int getPrice() const;
};

class normal : public delivery {
public:
    normal(int, int, int, QString, QString,QString,mytime, QString, mytime = mytime());
    virtual ~normal();
    virtual int getType() const;
    virtual int getPrice() const;
};

#endif // DELIVERY_H
