#include "registerpage.h"
#include "ui_registerpage.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QRegExpValidator>

/**
 * @brief 构造函数，进行页面初始化，信号和槽的连接等工作
 * @param parent 父窗口指针
 */
registerpage::registerpage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registerpage)
{
    //设置控件，设置页面无边框，背景透明，限制用户名只能为英文字母组成
    ui->setupUi(this);
    ui->username->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z]+$"),this));
    ui->username->setPlaceholderText("Only english characters");
    ui->password->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z]+$"),this));
    ui->password->setPlaceholderText("Only english characters");
    ui->telephone->setValidator(new QRegExpValidator(QRegExp("^[0-9]+$"),this));
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    QPushButton* cancelbtn = ui->cancel;
    QPushButton* OKbtn = ui->OK;

    //设置窗口阴影效果
    QGraphicsDropShadowEffect *windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(10);
    windowShadow->setColor(QColor(0, 0, 0));
    windowShadow->setOffset(0, 0);
    ui->mainwidget->setGraphicsEffect(windowShadow);

    //点击取消按钮时，关闭注册页面，显示登录界面
    connect(cancelbtn,&QPushButton::clicked,this,[=](){this->close();parentWidget()->show();});
    connect(OKbtn,&QPushButton::clicked,this,&registerpage::OKclicked);
}

/**
 * @brief 析构函数，释放控件
 */
registerpage::~registerpage()
{
    delete ui;
}

/**
 * @brief 点击OK按钮后获取注册信息
 */
void registerpage::OKclicked(){
    QString confirmPwd = ui->comfirm->text();
    _username = ui->username->text();
    _pwd = ui->password->text();
    _name = ui->name->text();
    _telephone = ui->telephone->text();
    _location = ui->location->text();

    //如果任何一项未填写则进行提示
    if(_username.isEmpty()||_location.isEmpty()||
            _name.isEmpty()||_telephone.isEmpty()){
        QMessageBox::critical(this,"Error",
                         "Some information has not been filled in");
    }
    //如果密码长度过短。给出提示
    else if(_pwd.size()<6){
        QMessageBox::critical(this,"Error",
                         "Your password is too short(least 6 characters)");
    }
    //正常则发出信号，检测用昵称是否唯一
    else if(_pwd == confirmPwd)
        emit isUnique(_username,_pwd);
    //两次密码输入不一致，给出提示并清空确认密码输入框中的文本
    else{
        QMessageBox::critical(this,"Error",
                         "The two passwords are inconsistent!");
        ui->comfirm->clear();
    }
}

/**
 * @brief 注册成功则写入用户文件中，否则提醒注册失败
 * @param success
 */
void registerpage::handle(bool success){
    if(success){
        QFile outFile(userpath);
        qDebug()<<outFile.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
        QTextStream fout(&outFile);
        fout << _username << endl;
        //新用户余额初始为0
        fout << 0 << endl;
        fout << _pwd << endl;
        fout << _name << endl;
        fout << _location << endl;
        fout << _telephone << endl;
        outFile.close();
        //关闭注册页面，显示登录界面
        this->close();
        parentWidget()->show();
    }else{
        //提示用户昵称已被注册
        QMessageBox::critical(this,"Error",
                              "This username was registered!");
    }
}
