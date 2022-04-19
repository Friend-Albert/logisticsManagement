#ifndef SENDPAGE_H
#define SENDPAGE_H

#include <QWidget>
#include <QString>

namespace Ui {
class sendpage;
}

class sendpage : public QWidget
{
    Q_OBJECT

signals:
    //传递收件人和快递描述
    void trySend(QString,QString);
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
