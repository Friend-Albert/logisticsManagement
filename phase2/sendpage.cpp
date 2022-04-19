#include "sendpage.h"
#include "ui_sendpage.h"

/**
 * @brief 初始化控件以及完成信号和槽的连接
 * @param parent 父窗口的指针
 */
sendpage::sendpage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sendpage)
{
    ui->setupUi(this);
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
    QString desc,addressee;
    desc = ui->lineEdit->text();
    addressee = ui->lineEdit_2->text();
    emit trySend(addressee,desc);
}
