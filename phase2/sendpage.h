#ifndef SENDPAGE_H
#define SENDPAGE_H

#include <QDialog>
#include <QString>
#include <QRegExpValidator>

namespace Ui {
class sendpage;
}

class sendpage : public QDialog
{
    Q_OBJECT

signals:
    //传递收件人和快递描述
    void trySend(QString,QString,int,int);
public:
    //构造函数
    explicit sendpage(QWidget *parent = nullptr);
    //析构函数
    ~sendpage();

private:
    Ui::sendpage *ui;
public slots:
    //点击发送按钮后发送trysend信号
    void sendClicked();
};

#endif // SENDPAGE_H
