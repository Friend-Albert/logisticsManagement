#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QDialog>
#include <QPushButton>
#include <QTextStream>
#include <QMap>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QColor>
#include <QPoint>
#include <QMouseEvent>
#include "registerpage.h"

#define passwdpath "D:/logisticsManagement/res/passwd.txt"

namespace Ui {
class loginpage;
}

class loginpage : public QDialog
{
    Q_OBJECT

signals:
    //传递登录用户的昵称的信号
    void loginsuccess(QString username,QString password);
    //传递注册是否成功的信号
    void registerRlt(bool status);
public:
    //界面的构造函数
    explicit loginpage(QWidget *parent = nullptr);
    //析构函数
    ~loginpage();

private:
    //存储当前界面UI控件的指针
    Ui::loginpage *ui;
    //注册页面的指针
    registerpage* newuser;
    //存储所有用户密码的哈希表
    QMap<QString,QString> passwd;
    //UI设置所需参数
    int cornerRadius = 20;
    //UI设置所需参数
    QColor mainBackGround = QColor(251,251,251);
    //UI设置所需参数
    QPoint mousePressedPosition;
    //UI设置所需参数
    QPoint windowPositionAsDrag;
    //读入用户名和密码并初始化UI
    void init();
    //判断是否成功登陆
    bool login();
    //保存所有的用户名和密码
    void save();

public slots:
    //尝试登录
    void trylogin();
    //隐藏登录界面，唤出注册界面
    void showRegister();
    //检查新注册用户昵称是否唯一
    void checkUnique(QString ,QString );
    //处理用户更改密码
    void changepwd(QString,QString);
};

#endif // LOGINPAGE_H
