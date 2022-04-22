#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QDialog>
#include <QPair>
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
#include "userpage.h"
#include "adminpage.h"

#define PASSWDPATH "D:/project/logisticsManagement/phase2/res/passwd.txt"
#define ADMIN 0
#define USER 1
#define POSTMAN 2

namespace Ui {
class loginpage;
}

class loginpage : public QDialog
{
    Q_OBJECT

signals:
    //管理员登录成功
    void adminLogin();
    //快递员登陆成功
    void postmanLogin(QString name);
    //传递注册是否成功的信号
    void registerRlt(bool status,int userType);
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
    userpage* userbox;
    adminpage* adminbox;
    //存储所有用户密码的哈希表
    QMap<QString,QPair<int,QString>> passwd;
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
