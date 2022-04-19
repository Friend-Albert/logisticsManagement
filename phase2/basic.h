#ifndef BASIC_H
#define BASIC_H

#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QString>

//用于表示时间的类
class mytime {
private:
    int _month;
    int _date;
    int _hour;
    int _minute;

public:
    //默认构造函数
    mytime();
    //拷贝构造函数
    mytime(const mytime& other);
    //手动输入月时分秒的普通构造函数
    mytime(int month, int day, int hour, int minute);
    //设置为当前系统时间
    void getRealTime();
    //获得对象的月份
    int getMonth() const ;
    //获得对应对象的日期
    int getDate() const ;
    //获得对应对象的小时数
    int getHour() const ;
    //获得对应对象的分钟数
    int getMin() const ;
    //判断日期合法性的函数
    bool isValid() const ;
    //输出指定格式的时间字符串
    QString toString();
    //判断是否同天
    bool isSameDay(QString);
    //重载赋值运算符
    mytime &operator=(const mytime &other);
    //重载判等运算符
    bool operator==(const mytime &other);
};

#endif // BASIC_H
