#ifndef PSTMPAGE_H
#define PSTMPAGE_H

#define POSTMENPATH "D:/project/logisticsManagement/phase2/res/postmen.txt"
#define DELIVERYPATH "D:/project/logisticsManagement/phase2/res/delivery.txt"
#define TEMPPATH "D:/project/logisticsManagement/phase2/res/Tmp.txt"
#define USERPATH "D:/project/logisticsManagement/phase2/res/user.txt"

#include <QWidget>
#include <QComboBox>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QTableWidget>
#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include "delivery.h"
#include "user.h"
#include "slidepage.h"

namespace Ui {
class pstmpage;
}

class pstmpage : public QDialog
{
    Q_OBJECT
signals:
    void sendQuery(QString);
public:
    explicit pstmpage(QString, QWidget *parent = nullptr);
    ~pstmpage();

private:
    Ui::pstmpage *ui;
    int _curRow = 0;
    SlidePage* _detail;
    user _postman;
    QVector<delivery*> _allDelivery;
    QVector<int> _showList;
    QString _postmanName;
    int _postmanBalance;
    int _adminBalance;
    void init();
    void showDelivery();
    void querypage();
    void queryPkg(QString);
    void flushList(int);
    void collectPkg();
    void save();
};

#endif // PSTMPAGE_H
