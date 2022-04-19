#include "basic.h"
/**
 * @brief 重载赋值运算符
 * @param other：拷贝的对象
 * @return 新对象的引用
 */
mytime& mytime::operator=(const mytime &other) {
        _month = other._month;
        _date = other._date;
        _hour = other._hour;
        _minute = other._minute;
        return *this;
}
/**
 * @brief 重载判等运算符
 * @param other：需要进行判断的对象
 * @return 两对象是否相等
 */
bool mytime::operator==(const mytime &other)
{
    return _month == other._month&&
           _date == other._date&&
           _hour == other._hour&&
           _minute == other._minute;
}
/**
 * @brief 获得时间的月份
 * @return 一个整数，代表月份
 */
int mytime::getMonth() const { return _month; }

/**
 * @brief 获得时间的日期
 * @return 一个整数，代表对象的日期
 */
int mytime::getDate() const { return _date; }

/**
 * @brief 获得时间的小时
 * @return 一个整数，代表对象的小时
 */
int mytime::getHour() const { return _hour; }

/**
 * @brief 获得时间的分钟数
 * @return 一个整数，代表对象的分钟数
 */
int mytime::getMin() const { return _minute; }

/**
 * @brief 判断时间是否合法
 * @return 一个bool值
 *         true代表合法
 *         false代表非法
 */
bool mytime::isValid() const
{
    bool rlt = true;
    char Month_buf[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };               //月份修正表
    if (_month>12||_month<1||_date>Month_buf[_month-1]||_date<1)           //判断月份日期是否合法
        rlt =  false;
    if(_hour<0||_hour>=24) rlt = false;
    if(_minute<0||_minute>=60) rlt = false;
    return rlt;
}

/**
 * @brief 将时间表示为字符串的转换函数
 * @return 表示时间的字符串，格式为"xx.xx xx:xx"
 */
QString mytime::toString()
{
    QString t;
    t = QString::number(_month) + "." + QString::number(_date) + " ";
    if(_hour<10){
        t += "0";
    }
    t += QString::number(_hour) + ":";
    if(_minute<10){
        t += "0";
    }
    t += QString::number(_minute);
    return t;
}

bool mytime::isSameDay(QString t)
{
    QString tmp = this->toString();
    for(int i=0;i<t.size();i++){
        if(tmp[i]!=t[i]) return  false;
    }
    return true;
}

/**
 * @brief 默认构造函数，将全部参数初始化为0
 */
mytime::mytime() : _month(0), _date(0), _hour(0), _minute(0) {}

/**
 * @brief 拷贝构造函数
 * @param other：需拷贝的对象
 */
mytime::mytime(const mytime& other) {
        _month = other._month;
        _date = other._date;
        _hour = other._hour;
        _minute = other._minute;
}

/**
 * @brief 有参构造函数
 * @param month 新对象的月份
 * @param day 新对象的日期
 * @param hour 新对象的小时
 * @param minute 新对象的分钟
 */
mytime::mytime(int month, int day, int hour, int minute) {
        _month = month;
        _date = day;
        _hour = hour;
        _minute = minute;
}

/**
 * @brief 将当前对象设置为电脑系统时间
 */
void mytime::getRealTime() {
    QDateTime sysTime;
    QDate date;
    QTime time;
    sysTime = QDateTime::currentDateTime();
    date = sysTime.date();
    time = sysTime.time();
    _month = date.month();
    _date = date.day();
    _hour = time.hour();
    _minute = time.minute();
}
