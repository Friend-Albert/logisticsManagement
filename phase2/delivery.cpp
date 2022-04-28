#include "delivery.h"

/**
 * @brief 默认构造函数，不进行任何初始化
 */
single::single()
{
    ;
}

/**
 * @brief 有参构造函数，用于新发送快递时
 * @param id 分配的快递单号
 * @param sender 寄件人姓名
 * @param addressee 收件人姓名
 * @param sendTime 发送时间
 * @param description 快递描述
 */
single::single(int id, QString sender, QString addressee,
               mytime sendTime,QString description) {
    _id = id;
    _sender = sender;
    _addressee = addressee;
    _sendTime = sendTime;
    _description = description;
}

/**
 * @brief 有参构造函数，用于从文件中读入快递时
 * @param id 快递单号
 * @param sender 寄件人姓名
 * @param addressee 收件人姓名
 * @param sendTime 发送时间
 * @param recvtime 收件时间
 * @param description 物品描述
 * @param hasSigned 是否签收
 */
single::single(int id, QString sender, QString addressee,
               mytime sendTime, mytime recvtime, QString description, int s) {
    _id = id;
    _sender = sender;
    _addressee = addressee;
    _sendTime = sendTime;
    _recvTime = recvtime;
    _description = description;
    _status = s;
}

/**
 * @brief 查看对象是否需要重新写入文件中（新创建/状态变化）
 * @return bool值
 *         true 需要重新写入文件中
 *         false 照搬文件中的原有信息即可
 */
bool delivery::getNeedRewrite() const
{
    return needRewrite;
}

/**
 * @brief 当快递刚创建/刚签收时设置该快递需要重新写入
 */
void delivery::setNeedRewrite()
{
    needRewrite = true;
}

/**
 * @brief delivery的默认构造函数，默认设置快递未签收
 */
const QString &delivery::getPostman() const
{
    return _postman;
}

void delivery::setPostman(const QString &newPostman)
{
    _postman = newPostman;
}

int delivery::getNumber() const
{
    return _number;
}

/**
 * @brief 设置快递已签收并且设置签收时间为当前系统时间
 */
void delivery::setSigned() {
    mytime recv;
    recv.getRealTime();
    _status = delivery::hasSigned;
    _recvTime = recv;
}

/**
 * @brief 获得快递单号
 * @return 一个整数，代表快递单号
 */
int delivery::getid() const { return _id; }

/**
 * @brief 获取寄件人
 * @return 寄件人昵称的字符串
 */
QString delivery::getSender() const { return _sender; }

/**
 * @brief 获取收件人
 * @return 收件人昵称的字符串
 */
QString delivery::getAddressee() const { return _addressee; }

/**
 * @brief 获取快递描述
 * @return 描述快递的字符串
 */
QString delivery::getDescription() const { return _description; }

/**
 * @brief 查看快递是否已签收
 * @return bool值
 *         true 已签收
 *         false 未签收
 */
int delivery::getStatus() const { return _status; }

void delivery::setStatus(const int status) { _status = status; }

/**
 * @brief 获取寄件时间
 * @return mytime类型的时间
 */
mytime delivery::getSendTime() const { return _sendTime; }

/**
 * @brief 获取收件时间
 * @return mytime类型的时间
 */
mytime delivery::getRecvTime() const { return _recvTime; }

/**
 * @brief 计算快递价格
 * @return 文档中说明本类快递价格固定为15元
 */
int single::getPrice() const {
    return 15;
}

int single::getType() const
{
    return 0;
}

book::book(int id, int n, int s,QString sender, QString addressee,
           QString postman, mytime send, QString desc, mytime recv)
{
    _id = id;
    _number = n;
    _status = s;
    _sender = sender;
    _addressee = addressee;
    _postman = postman;
    _sendTime = send;
    _description = desc;
    _recvTime = recv;
}

book::~book(){}

int book::getType() const
{
    return 1;
}

int book::getPrice() const
{
    return _number * 2;
}

fragile::fragile(int id, int n, int s,QString sender, QString addressee,
                 QString postman, mytime send, QString desc, mytime recv)
{
    _id = id;
    _number = n;
    _status = s;
    _sender = sender;
    _addressee = addressee;
    _postman = postman;
    _sendTime = send;
    _description = desc;
    _recvTime = recv;
}

fragile::~fragile(){}

int fragile::getType() const
{
    return 2;
}

int fragile::getPrice() const
{
    return _number * 8;
}

normal::normal(int id, int n, int s,QString sender, QString addressee,
               QString postman, mytime send, QString desc, mytime recv)
{
    _id = id;
    _number = n;
    _status = s;
    _sender = sender;
    _addressee = addressee;
    _postman = postman;
    _sendTime = send;
    _description = desc;
    _recvTime = recv;
}

normal::~normal(){}

int normal::getType() const
{
    return 3;
}

int normal::getPrice() const
{
    return _number * 5;
}
