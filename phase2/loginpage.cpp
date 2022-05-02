#include "loginpage.h"
#include "ui_loginpage.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QTimer>

/**
 * @brief 构造函数
 * @param parent 登录界面的父窗口
 */
loginpage::loginpage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginpage)
{
    //创建控件
    ui->setupUi(this);
    //堆上申请注册界面
    _register = new registerpage(this);

    QPushButton* loginbtn = ui->loginbtn;
    QPushButton* registerbtn = ui->registerbtn;
    QPushButton* quitbtn = ui->quit;
    QPushButton* minibtn = ui->minimize;

    //设置按钮的样式
    ui->passwdinput->setStyleSheet("background-color:rgba(251,251,251);border-top:0px;\
                                    border-bottom:2px  solid;border-left:0px;border-right: 0px;");
    ui->usernameinput->setStyleSheet("background-color:rgba(251,251,251);border-top:0px;\
                                      border-bottom:2px  solid;border-left:0px;border-right: 0px;");
    ui->quit->setStyleSheet("QPushButton{background-color:rgba(210,210,210,50%);border:2px;border-radius: 10px;}"
                            "QPushButton:hover{background-color:#ff6767;color:black;}");
    ui->minimize->setStyleSheet("QPushButton{background-color:rgba(210,210,210,50%);border:2px;border-radius: 10px;}"
                            "QPushButton:hover{background-color:#FFBF00;color:black;}");
    ui->loginbtn->setStyleSheet("QPushButton{background-color:rgba(50,120,255,50%);border-radius: 5px;}"
                                "QPushButton:hover{background-color:#3080ff;color:black;}");
    ui->registerbtn->setStyleSheet("QPushButton{background-color:rgba(210,210,210,50%);border-radius: 5px;}"
                                "QPushButton:hover{background-color:#000000;color:black;}");

    init();

    //登录按钮点击时，调用trylogin检查用户名和密码
    connect(loginbtn,&QPushButton::clicked,this,&loginpage::trylogin);
    //注册按钮点击时，隐藏登录界面并唤起注册界面
    connect(registerbtn,&QPushButton::clicked,this,&loginpage::showRegister);
    //退出按钮点击时，关闭窗口
    connect(quitbtn,&QPushButton::clicked,this,[=](){this->close();});
    //最小化按钮点击时，将窗口最小化
    connect(minibtn,&QPushButton::clicked,this,[=](){this->showMinimized();});
}

/**
 * @brief 析构函数，保存用户名和密码数据并释放所有控件
 */
loginpage::~loginpage()
{
    save();
    if(_user) delete _user;
    delete _register;
    delete ui;
}

/**
 * @brief 读入用户名和密码数据，设置窗体样式
 */
void loginpage::init(){
    //读取存储用户昵称和密码的文件，建立昵称和密码的哈希表
    QFile readFile(PASSWDPATH);
    if(!readFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "failed" <<endl;
        return ;
    }
    QTextStream fin(&readFile);
    QString username,pwd;
    int usernum = 0;
    int type;
    fin >> usernum;
    qDebug()<<usernum<<endl;
    fin.readLine();
    while(usernum--){
        fin >> username;
        fin >> type;
        if(!username.isNull()){
            fin >> pwd;
            passwd[username] = QPair<int,QString>(type,pwd);
        }
    }
    readFile.close();

    //设置窗体无边框且背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);

    //绘制圆角窗体
    QPainterPath path;
    path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
    QRegion mask(path.toFillPolygon().toPolygon());
    ui->mainWidget->setMask(mask);

    //设置窗体样式
    QString mainStyle;
    ui->mainWidget->setObjectName("mainWidget");
    mainStyle = "QWidget#mainWidget{background-color:" + mainBackGround.name() + QString::asprintf(";border-radius:%dpx", cornerRadius) + "}";
    ui->mainWidget->setStyleSheet(mainStyle);

    //设置窗体阴影
    QGraphicsDropShadowEffect *windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(20);
    windowShadow->setColor(QColor(0, 0, 0));
    windowShadow->setOffset(0, 0);
    ui->mainWidget->setGraphicsEffect(windowShadow);
    QWidget *border = new QWidget(this);
    border->move(ui->mainWidget->pos() - QPoint(1, 1));
    border->resize(ui->mainWidget->size() + QSize(2, 2));
    QString borderStyle;
    borderStyle = "background-color:#00FFFFFF;border:1.5px solid #686868; border-radius:" + QString::asprintf("%d",cornerRadius) + "px";
    border->setStyleSheet(borderStyle);
    border->setAttribute(Qt::WA_TransparentForMouseEvents);
    border->show();
}

/**
 * @brief 退出页面时保存用户昵称和密码数据
 */
void loginpage::save(){
    //打开密码文件，输出哈希表(passwd)中的所有用户名，密码
    QFile outFile(PASSWDPATH);
    outFile.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    QTextStream fout(&outFile);
    int usernum = passwd.size();
    fout << usernum << endl;
    for(auto i = passwd.begin();i!=passwd.end();i++){
        fout << i.key() << " " << i.value().first << endl;
        fout << i.value().second << endl;
    }
    outFile.close();
}

/**
 * @brief 检查用户名和密码是否匹配
 * @return bool
 *         true 匹配
 *         false 不匹配
 */
bool loginpage::login(){
    int type;
    QString username = ui->usernameinput->text();
    QString pwd = ui->passwdinput->text();
    if(ui->adminbtn->isChecked()) type = ADMIN;
    else if(ui->postmanbtn->isChecked()) type = POSTMAN;
    else type = USER;
    if(passwd.find(username)!=passwd.end()){
        if(passwd[username].second == pwd && passwd[username].first == type)
            return true;
    }
    return false;
}

/**
 * @brief 如果匹配，显示对应窗口，不匹配则弹窗告知
 */
void loginpage::trylogin(){
    bool success = false;
    bool isAdmin = ui->adminbtn->isChecked();
    bool isUser = ui->userbtn->isChecked();
    bool isPostman = ui->postmanbtn->isChecked();
    success = login();
    if(success){
        if(isUser){
            _user = new userpage(ui->usernameinput->text(),ui->passwdinput->text(),this);
            this->hide();
            _user->show();
            //当在用户界面更改密码后更改于此存储的用户密码表
            connect(_user,&userpage::newPwd,this,&loginpage::changepwd);
        }
        else if(isAdmin){
            _admin = new adminpage(this);
            this->hide();
            _admin->show();
            connect(_admin->getNewPostman(),&registerpage::sendInfo,this,&loginpage::addPwd);
        }
        else if(isPostman){
            _postman = new pstmpage(ui->usernameinput->text(),this);
            this->hide();
            _postman->show();
        }
        ui->passwdinput->clear();
        ui->usernameinput->clear();
    }else{
        QMessageBox::critical(this,"Wrong",
                       "username/password wrong!");
    }
}

/**
 * @brief 隐藏当前窗口，显示注册页面
 */
void loginpage::showRegister(){
    this->hide();
    _register->show();
    //注册页面尝试注册，调用checkUnique检测新用户昵称是否唯一
    connect(_register,&registerpage::sendInfo,this,&loginpage::checkUnique);
    connect(this,&loginpage::registerRlt,_register,&registerpage::handle);
}

/**
 * @brief 接收新注册的昵称和密码检查昵称是否唯一
 * @param username 新注册用户昵称
 * @param pwd 新注册用户密码
 */
void loginpage::checkUnique(QString username,QString pwd){
    bool success = false;
    if(passwd.find(username)==passwd.end()){
        success = true;
        //注册成功将用户名和密码加入哈希表中
        passwd[username] = QPair<int,QString>(USER,pwd);
    }
    //发送注册结果的信号
    emit registerRlt(success, USER);
}

void loginpage::changepwd(QString username, QString newpwd)
{
    passwd[username] = QPair<int,QString>(USER,newpwd);
}

void loginpage::addPwd(QString username, QString pwd)
{
    passwd[username] = QPair<int,QString>(POSTMAN,pwd);
}
