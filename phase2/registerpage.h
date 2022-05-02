#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QDialog>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include "user.h"

#define USERPATH "D:/project/logisticsManagement/phase2/res/user.txt"

namespace Ui {
class registerpage;
}

class registerpage : public QDialog
{
    Q_OBJECT

signals:
    //传递注册昵称和密码
    void sendInfo(QString username,QString pwd);
public:
    //构造函数
    explicit registerpage(QWidget *parent = nullptr);
    //析构函数
    ~registerpage();

private:
    //存储控件的指针
    Ui::registerpage *ui;
    //注册用户的姓名
    QString _name;
    //注册用户的昵称
    QString _username;
    //注册用户的电话
    QString _telephone;
    //注册用户的密码
    QString _pwd;
    //注册用户的地址
    QString _location;
    //临时生成的用户
    user _temp;

public slots:
    //点击完成按钮时获取用户名和密码并发出isUnique信号
    void OKclicked();
    //根据注册是否成功进行处理
    void handle(bool, int);
};

#endif // REGISTERPAGE_H
