#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#define POSTMENPATH "D:/project/logisticsManagement/phase2/res/postmen.txt"
#define DELIVERYPATH "D:/project/logisticsManagement/phase2/res/delivery.txt"
#define TEMPPATH "D:/project/logisticsManagement/phase2/res/Tmp.txt"
#define USERPATH "D:/project/logisticsManagement/phase2/res/user.txt"


#include <QWidget>
#include <QComboBox>
#include <QMap>
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
#include "registerpage.h"

namespace Ui {
class adminpage;
}

class adminpage : public QDialog
{
    Q_OBJECT
signals:
    void postmanRegister(bool, int);
    void statusChange(QString);
    void sendQuery(QString);
public:
    explicit adminpage(QWidget *parent = nullptr);
    ~adminpage();

    registerpage *getNewPostman() const;

private:
    Ui::adminpage *ui;
    int nextId;
    int curRow = 0;
    int userNumber = 0;
    user admin;
    QVector<delivery*> allDelivery;
    QVector<int> showList;
    QSet<QString> postmanName;
    QMap<int,int> rowId;
    registerpage* newPostman = nullptr;
    void init();
    void save();
    void showDelivery(QTableWidget*, const QVector<int>&);
    void flushList(int row);
    void queryDetail();
    void assignPage();
    void assignTask(QString);
    void needAssign(int row);
    void queryPkg(QString);
    void queryPage();
    void handle();
};

#endif // ADMINPAGE_H
