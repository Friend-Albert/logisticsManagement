#include "sendpage.h"
#include "ui_sendpage.h"

/**
 * @brief 初始化控件以及完成信号和槽的连接
 * @param parent 父窗口的指针
 */
sendpage::sendpage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sendpage)
{
    ui->setupUi(this);
    ui->lineEdit_3->setValidator(new QRegExpValidator(QRegExp("^[0-9]+$"),this));
    //点击取消按钮时清空输入框内容并且关闭窗口
    connect(ui->Cancelbtn,&QPushButton::clicked,this,[&](){
        this->close();
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
    });
    //点击寄送按钮时发送快递描述和收件人字符串
    connect(ui->OKbtn,&QPushButton::clicked,this,&sendpage::sendClicked);
}

/**
 * @brief 释放控件
 */
sendpage::~sendpage()
{
    delete ui;
}

/**
 * @brief 发送收件人和快递描述信息
 */
void sendpage::sendClicked()
{
    QString desc,addressee,n;
    int number,type;
    desc = ui->lineEdit->text();
    addressee = ui->lineEdit_2->text();
    n = ui->lineEdit_3->text();
    if(ui->bookbtn->isChecked()) type = 1;
    else if(ui->fgbtn->isChecked()) type = 2;
    else type = 3;
    number = n.toInt();
    emit trySend(addressee,desc,number,type);
}
