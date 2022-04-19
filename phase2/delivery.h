#ifndef DELIVERY_H
#define DELIVERY_H

#include <QString>
#include "basic.h"

//所有快递种类的基类
class delivery {

protected:
    //用于区别快递的唯一ID
    int _id;
    //快递寄件人的USERID以及用户名
    QString _sender;
    //快递收件人的USERID以及用户名
    QString _addressee;
    //是否签收
    bool _hasSigned;
    //寄件时间
    mytime _sendTime;
    //收件时间
    mytime _recvTime;
    //快递描述
    QString _description;
    //快递信息是否发生了更改，需要更新文件中的信息
    bool needRewrite = false;

public:
    //默认构造函数
    delivery();
    //设置快递签收（传入签收时间）
    void setSigned();
    //计算价格的纯虚函数
    virtual int calPrice() const = 0;
    //获得快递ID
    int getid() const ;
    //获得寄件人信息
    QString getSender() const ;
    //获得收件人信息
    QString getAddressee() const ;
    //获得快递描述
    QString getDescription() const ;
    //获得快递状态（是否签收）
    bool status() const ;
    //获得快递寄件时间
    mytime getSendTime() const ;
    //获得快递收件时间
    mytime getRecvTime() const ;
    //获取是否需要重写
    bool getNeedRewrite() const;
    //设置需要重写
    void setNeedRewrite();
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
           mytime, mytime, QString,bool);
    //父类计算快递价格函数的重写
    virtual int calPrice() const;
};

#endif // DELIVERY_H
