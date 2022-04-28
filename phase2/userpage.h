#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define DELIVERYPATH "D:/project/logisticsManagement/phase2/res/delivery.txt"
#define TEMPPATH "D:/project/logisticsManagement/phase2/res/Tmp.txt"
#define USERPATH "D:/project/logisticsManagement/phase2/res/user.txt"

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QVector>
#include <QSet>
#include <QMap>
#include <QTableWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include "user.h"
#include "delivery.h"
#include "sendpage.h"
#include "slidepage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class userpage : public QMainWindow
{
    Q_OBJECT
signals:
    //发送用户名和新密码
    void newPwd(QString,QString);
    //充值信号
    void tryRecharge(QLabel*);
    //发送查询请求文本
    void sendQuery(QString);
public:
    //构造函数
    userpage(QString username,QString pwd,QWidget *parent = nullptr);
    //析构函数
    ~userpage();
    //当前用户的信息
    user curUser;
    //管理员账户，用于管理余额
    user admin;
    //获得当前用户的昵称
    const QString &username() const;
    //设置当前用户的昵称
    void setUsername(const QString &newUsername);

private:
    Ui::MainWindow *ui;
    //寄快递页面的指针
    sendpage* sendbox;
    //用户详情页的指针
    SlidePage* detail = nullptr;
    //当前用户是否为管理员
    bool isAdmin = false;
    //tableWidget的列数（常量）
    const int tableColumns = 8;
    //当前操作的tableWidget的行索引
    int curRow = 0;
    //用来记录下一个新快递的单号
    int nextId = 0;
    //当前用户的用户名
    QString _username;
    //当前用户的密码
    QString _password;
    //所有用户的昵称（用于判断收件人是否存在）
    QSet<QString> _allUsername;
    //与当前用户关联的所有快递
    QVector<delivery*> _allDelivery;
    //当前应当显示的快递的索引
    QVector<int> _showList;
    //记录快递单号同tableWidget行数的对应关系
    QMap<int,int> _rowId;
    //初始化界面等操作
    void init();
    //保存数据
    void save();
    //tableWidget中显示指定快递
    void showDelivery(QTableWidget*,const QVector<int>&);
public slots:
    //刷新右侧列表显示选中快递信息
    void flushList(int row);
    //判断当前行的快递是否能签收
    void needSigned(int row);
    //签收快递
    void signPkg();
    //寄新快递
    void sendPkg(QString,QString,int,int);
    //更改密码
    void changePwd();
    //更改余额
    void balance(QLabel*);
    //创建查询快递页面
    void queryPage();
    //查询快递并展示
    void queryPkg(QString);
};
#endif // MAINWINDOW_H
