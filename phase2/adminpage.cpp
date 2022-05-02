#include "adminpage.h"
#include "ui_adminpage.h"

adminpage::adminpage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adminpage)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    newPostman = new registerpage(this);
    init();
    ui->assignbtn->setEnabled(false);

    connect(ui->assignbtn,&QPushButton::clicked,this,&adminpage::assignPage);
    connect(this,&adminpage::statusChange,this,&adminpage::assignTask);
    connect(ui->querybtn,&QPushButton::clicked,this,&adminpage::queryPage);
    connect(this,&adminpage::sendQuery,this,&adminpage::queryPkg);
    connect(ui->infobtn,&QPushButton::clicked,this,&adminpage::queryDetail);
    connect(ui->postmanbtn,&QPushButton::clicked,this,&adminpage::handle);
    connect(ui->quitbtn,&QPushButton::clicked,this,[=](){
        this->close();
        this->parentWidget()->show();
    });
}

adminpage::~adminpage()
{
    save();
    for(int i=0;i<allDelivery.size();i++)
        delete allDelivery[i];
    delete ui;
}

registerpage *adminpage::getNewPostman() const
{
    return newPostman;
}

void adminpage::init()
{
    QFile inFile(USERPATH);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream fin(&inFile);
    QString username,name,tele,loc,pwd;
    int balance,type;
    while(!fin.atEnd()){
        fin >> username;
        if(username.isEmpty()) break;
        fin >> balance >> type;
        fin >> pwd;
        fin.read(1);
        name = fin.readLine();
        fin >> tele;
        fin.read(1);
        loc = fin.readLine();
        ++userNumber;
        qDebug() << username <<endl;
        if(type == 0) {
            admin = user(balance,username,name,loc,tele);
        }else if(type == 2){
            postmanName.insert(username);
        }
    }
    inFile.close();

    nextId = 0;
    QString sender,addressee,desc,postman;
    int id,status,n;
    int smonth,sday,shour,smin;
    int rmonth,rday,rhour,rmin;
    QFile deliFile(DELIVERYPATH);
    deliFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream in(&deliFile);
    while(!in.atEnd()){
        desc = in.readLine();
        if(desc.isEmpty()) break;
        in >> id >> type >> n >> sender >> addressee;
        in >> postman;
        in >> status;
        in >> smonth >> sday >> shour >> smin >> rmonth >> rday >> rhour >> rmin;
        in.read(1);
        ++nextId;
        //qDebug() << id << endl;
        mytime st(smonth,sday,shour,smin),rt(rmonth,rday,rhour,rmin);
        delivery* tmp;
        if(type == 1)
            tmp = new book(id,n,status,sender,addressee,postman,st,desc,rt);
        else if(type == 2)
            tmp = new fragile(id,n,status,sender,addressee,postman,st,desc,rt);
        else if(type == 3)
            tmp = new normal(id,n,status,sender,addressee,postman,st,desc,rt);
        allDelivery.push_back(tmp);
        showList.push_back(allDelivery.size()-1);
    }
    deliFile.close();
    ui->tableWidget->setColumnCount(8);
    //显示所有关联快递
    showDelivery(ui->tableWidget,showList);

    //设置tableWidget选中模式为整行选中并且不允许编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置表头标题
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Description"
        << "Type" << "Number" << "Sender" << "Recipient"
        << "Postman" << "Send Time"<<"Receive Time");

    //当点击单元格时更新列表信息为当前信息
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&adminpage::flushList);
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&adminpage::needAssign);
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,[=](int row){curRow = row;});
    //当单元格信息发生更改时更新列表信息
    connect(ui->tableWidget,&QTableWidget::cellChanged,this,&adminpage::flushList);
}

void adminpage::save()
{
    //存储已经写入的快递的id
    QSet<int> rewrite;
    QString sender,addressee,desc,postman;
    int id,status,type,n;
    int smonth,sday,shour,smin;
    int rmonth,rday,rhour,rmin;
    QFile inFile(DELIVERYPATH);
    inFile.open(QIODevice::Text|QIODevice::ReadOnly);
    QTextStream fin(&inFile);
    QFile outFile(TEMPPATH);
    outFile.open(QIODevice::Text|QIODevice::Truncate|QIODevice::WriteOnly);
    QTextStream fout(&outFile);
    //写入所有信息变更/新创建的快递
    for(int i=0;i<allDelivery.size();i++){
        if(allDelivery[i]->getNeedRewrite()){
            delivery& dly = *allDelivery[i];
            rewrite.insert(dly.getid());
            fout << dly.getDescription() << endl;
            fout << dly.getid() <<" "<<dly.getType() << " " <<dly.getNumber();
            fout << " "<< dly.getSender()<< " " <<dly.getAddressee() << endl;
            fout << dly.getPostman() << endl;
            fout << dly.getStatus() << endl;
            fout << dly.getSendTime().getMonth() <<" "<< dly.getSendTime().getDate() << " "
                 << dly.getSendTime().getHour() <<" "<<dly.getSendTime().getMin()<<" ";
            fout << dly.getRecvTime().getMonth() <<" "<< dly.getRecvTime().getDate()<<" "
                 << dly.getRecvTime().getHour() <<" "<<dly.getRecvTime().getMin() << endl;
        }
    }
    //将原有文件信息拷贝入临时文件中
    while(!fin.atEnd()){
        desc = fin.readLine();
        if(desc.isEmpty()) break;
        fin >> id >> type >> n >> sender >> addressee;
        fin >> postman;
        fin >> status;
        fin >> smonth >> sday >> shour >> smin >> rmonth >> rday >> rhour >> rmin;
        fin.read(1);
        //如果该快递已经写入了文件中（信息发生更改）则跳过
        if(rewrite.find(id) != rewrite.end()) continue;
        //否则原样写入临时文件中
        fout << desc << endl;
        fout << id << " " << type << " " <<n << " " <<sender<<" "<< addressee << endl;
        fout << postman<<endl;
        fout << status << endl;
        fout << smonth <<" "<<sday<<" "<<shour<<" "
             << smin <<" "<<rmonth<<" "<<rday<<" "
             << rhour <<" "<< rmin<<endl;
    }
    inFile.close();
    outFile.close();
    //删除原文件并更名临时文件
    inFile.remove();
    outFile.rename(DELIVERYPATH);
}

/**
 * @brief 依照索引显示对应快递
 * @param table 需要显示快递信息的tableWidget指针
 * @param all 需要显示快递的索引
 */
void adminpage::showDelivery(QTableWidget *table, const QVector<int> &all)
{
    //要先断开单元格内容变更同更新列表的连接，否则会导致程序崩溃
    disconnect(table,&QTableWidget::cellChanged,this,&adminpage::flushList);

    //清空原有表格内容
    for(int row = table->rowCount() - 1;row >= 0; row--){
        table->removeRow(row);
    }

    //按表格形式显示快递
    int curRow = 0;
    for(int i=0;i<all.size();i++){
        int idx = all[i];
        table->insertRow(curRow);
        rowId[curRow] = allDelivery[idx]->getid();
        delivery& d = *allDelivery[idx];
        table->setItem(curRow,0,new QTableWidgetItem(d.getDescription()));
        if(d.getType() == 1)
            table->setItem(curRow,1,new QTableWidgetItem("Book"));
        else if(d.getType() == 2)
            table->setItem(curRow,1,new QTableWidgetItem("Fragile"));
        else if(d.getType() == 3)
            table->setItem(curRow,1,new QTableWidgetItem("Normal"));
        table->setItem(curRow,2,new QTableWidgetItem(QString::number(d.getNumber())));
        table->setItem(curRow,3,new QTableWidgetItem(d.getSender()));
        table->setItem(curRow,4,new QTableWidgetItem(d.getAddressee()));
        table->setItem(curRow,5,new QTableWidgetItem(d.getPostman()));
        table->setItem(curRow,6,new QTableWidgetItem(d.getSendTime().toString()));
        if(d.getStatus() & delivery::hasSigned)
            table->setItem(curRow,7,new QTableWidgetItem(d.getRecvTime().toString()));
        else if(d.getStatus() & delivery::toBeSigned)
            table->setItem(curRow,7,new QTableWidgetItem("Not yet signed"));
        else
            table->setItem(curRow,7,new QTableWidgetItem("Not yet collected"));
       ++curRow;
    }

    //重新将单元格内容更改同刷新列表相连
    connect(table,&QTableWidget::cellChanged,this,&adminpage::flushList);
}

void adminpage::flushList(int row)
{
    QListWidget* list = ui->listWidget;
    QTableWidget* table = ui->tableWidget;
    //如果已经创建了组件，则更改组件文本
    if(list->count()>0){
        list->item(0)->setText("Description: "+table->item(row,0)->text());
        list->item(1)->setText("Type: "+table->item(row,1)->text());
        list->item(2)->setText("Number: "+table->item(row,2)->text());
        list->item(3)->setText("Sender: "+table->item(row,3)->text());
        list->item(4)->setText("Recipient: "+table->item(row,4)->text());
        list->item(5)->setText("Postman: "+table->item(row,5)->text());
        list->item(6)->setText("SendTime: "+table->item(row,6)->text());
        list->item(7)->setText("ReceiveTime: "+table->item(row,7)->text());
    }
    //否则创建组件
    else{
        list->addItem("Description: "+table->item(row,0)->text());
        list->addItem("Type: "+table->item(row,1)->text());
        list->addItem("Number: "+table->item(row,2)->text());
        list->addItem("Sender: "+table->item(row,3)->text());
        list->addItem("Recipient: "+table->item(row,4)->text());
        list->addItem("Postman: "+table->item(row,5)->text());
        list->addItem("SendTime: "+table->item(row,6)->text());
        list->addItem("ReceiveTime: "+table->item(row,7)->text());
    }
}

/**
 * @brief 管理员模式下显示所有用户信息(包括快递员)
 */
void adminpage::queryDetail()
{
    //设置页面组件并且设置表格无法更改
    QDialog *user = new QDialog(this);
    user->resize(600,400);
    user->setAttribute(Qt::WA_DeleteOnClose);
    user->setWindowTitle("User Information");
    QVBoxLayout* mainLayout = new QVBoxLayout(user);
    QTableWidget* detail = new QTableWidget(userNumber,7,user);
    detail->resize(600,350);
    detail->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QLineEdit* input = new QLineEdit(user);
    input->resize(500,50);
    input->setPlaceholderText("Please input username to query certain user");
    mainLayout->addWidget(detail);
    mainLayout->addWidget(input);
    //设置表头
    detail->setHorizontalHeaderLabels(QStringList()<<"username"<<"password"<<"name"<<"balance"
                                      <<"tele."<<"location"<<"type");
    //读入所有用户信息
    QFile userFile(USERPATH);
    userFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream fin(&userFile);
    QString username,name,tele,loc,pwd;
    int balance,cur = 0,type;
    while(!fin.atEnd()){
        fin >> username;
        if(username.isEmpty()) break;
        fin >> balance >> type;
        fin >> pwd;
        fin.read(1);
        name = fin.readLine();
        fin >> tele;
        fin.read(1);
        loc = fin.readLine();
        //将用户信息填入表格中
        detail->setItem(cur,0,new QTableWidgetItem(username));
        detail->setItem(cur,1,new QTableWidgetItem(pwd));
        detail->setItem(cur,2,new QTableWidgetItem(name));
        detail->setItem(cur,3,new QTableWidgetItem(QString::asprintf("%d",balance)));
        detail->setItem(cur,4,new QTableWidgetItem(tele));
        detail->setItem(cur,5,new QTableWidgetItem(loc));
        if(type == 0)
            detail->setItem(cur,6,new QTableWidgetItem("Admin"));
        else if(type == 1)
            detail->setItem(cur,6,new QTableWidgetItem("User"));
        else if(type == 2)
            detail->setItem(cur,6,new QTableWidgetItem("Postman"));
        ++cur;
    }
    userFile.close();
    user->show();

    //按下回车键时查询特定用户信息
    connect(input,&QLineEdit::returnPressed,user,[=](){
        QString queryText = input->text();
        //输入为空时，显示所有用户信息
        if(queryText.size() == 0){
            for(int i=0;i<userNumber;i++){
                detail->setRowHidden(i,false);
            }
        }
        //不为空时，依照用户名进行匹配
        else{
            for(int i=0;i<userNumber;i++){
                if(queryText != detail->item(i,0)->text()){
                    detail->setRowHidden(i,true);
                }
            }
        }
    });
}

void adminpage::assignPage()
{
    QDialog* assign = new QDialog(this);
    assign->setAttribute(Qt::WA_DeleteOnClose);
    assign->resize(600,400);
    QVBoxLayout* mainLayout = new QVBoxLayout(assign);
    QHBoxLayout* btn = new QHBoxLayout();
    QComboBox* postman = new QComboBox();
    QPushButton* okbtn = new QPushButton("OK");
    QPushButton* cancelbtn = new QPushButton("Cancel");
    for(auto i = postmanName.begin();i!=postmanName.end();i++){
        postman->addItem(*i);
    }
    btn->addWidget(okbtn);
    btn->addWidget(cancelbtn);
    mainLayout->addWidget(postman);
    mainLayout->addLayout(btn);
    assign->show();
    connect(okbtn,&QPushButton::clicked,assign,[=](){
        assign->close();
        emit statusChange(postman->currentText());
    });
    connect(cancelbtn,&QPushButton::clicked,assign,[=](){assign->close();});
}

void adminpage::assignTask(QString postmanName)
{
    int index = showList[curRow];
    allDelivery[index]->setPostman(postmanName);
    allDelivery[index]->setNeedRewrite();
    ui->tableWidget->item(curRow,5)->setText(postmanName);
    ui->querybtn->setEnabled(false);
}

void adminpage::needAssign(int row)
{
    QTableWidget* table = ui->tableWidget;
    QString status = table->item(row,7)->text();
    QString postman = table->item(row,5)->text();
    //如果需要分配快递员，则设置分配按钮可以点击
    if(status.contains("collected") && postman == "None"){
        ui->assignbtn->setEnabled(true);
    }
    else
        ui->assignbtn->setEnabled(false);
}

/**
 * @brief 显示查询快递页面
 */
void adminpage::queryPage()
{
    QDialog *query = new QDialog(this);
    query->setAttribute(Qt::WA_DeleteOnClose);
    query->resize(500,400);
    query->setWindowTitle("Query Delivery");
    QLabel *rule = new QLabel(query);
    QLineEdit *input = new QLineEdit(query);
    QVBoxLayout *mainLayout = new QVBoxLayout(query);
    //声明查询规则
    rule->setText(QString("Query Rules:\nIf you query by time,please enter it in the following ") +
        QString("format:Month.Date\nIf you query by sender/recipient,please input sender/recipient:his/her username,") +
        QString("like sender:user1.\nIf you query by courier number please input an interger\nif you input nothing") +
        QString(",then all deliveries will be shown.\nIf you query by state,please input:toBeCollected\\") +
        QString("toBeSigned\\signed"));
    rule->setFont(QFont("Corbel",12));
    rule->setWordWrap(true);
    mainLayout->addWidget(rule);
    mainLayout->addWidget(input);
    query->show();
    //输入栏输入回车时则关闭窗口并处理查询请求
    connect(input,&QLineEdit::returnPressed,query,[=](){
        query->close();
        emit sendQuery(input->text());
    });
}

void adminpage::handle()
{
    newPostman->show();
    this->hide();
    connect(newPostman,&registerpage::sendInfo,this,[=](QString username){
       if(postmanName.find(username) == postmanName.end())
           emit postmanRegister(true,2);
       else
           emit postmanRegister(false,2);
    });
    connect(this,&adminpage::postmanRegister,newPostman,&registerpage::handle);
}

/**
 * @brief 依照不同的查询文本格式给出对应结果
 * @param 查询输入的文本
 */
void adminpage::queryPkg(QString text)
{
    int isStatus = 0;
    bool isDate = false;
    bool isNumber = false;
    int courierN = text.toInt(&isNumber);
    //清空展示快递的数组
    showList.clear();
    //只有查询日期时才会出现.字符，以此判断是否查询日期
    for(int i=0;i<text.size();i++){
        if(text[i]=='.') {isDate = true;break;}
    }
    //判断是否在查询物品状态
    if(text == "toBeCollected")
        isStatus = 0x01;
    else if( text == "toBeSigned")
        isStatus = 0x02;
    else if(text == "signed")
        isStatus = 0x04;

    //如果输入为空，则全部显示
    if(text.isEmpty()){
        for(int i=0;i<allDelivery.size();i++) showList.push_back(i);
    }
    //如果输入日期，则查找对应快递
    else if(isDate){
        for(int i=0;i<allDelivery.size();i++){
            mytime t1 = allDelivery[i]->getSendTime();
            mytime t2 = allDelivery[i]->getRecvTime();
            if(t1.isSameDay(text)||t2.isSameDay(text)){
                showList.push_back(i);
            }
        }
    }
    //如果输入数字，则查找对应快递单号的快递
    else if(isNumber){
        for(int i=0;i<allDelivery.size();i++){
            if(courierN == allDelivery[i]->getid()) {showList.push_back(i); break;}
        }
    }
    else if(isStatus){
        for(int i=0;i<allDelivery.size();i++){
            if(allDelivery[i]->getStatus() == isStatus)
                showList.push_back(i);
        }
    }
    //剩下只可能输入用户名，则查找关联快递
    else{
        if(text.startsWith("sender:")){
            text = text.mid(7);
            for(int i=0;i<allDelivery.size();i++){
                if(text == allDelivery[i]->getSender())
                    showList.push_back(i);
                }
        }
        else if(text.startsWith("recipient:")){
            text = text.mid(10);
            for(int i=0;i<allDelivery.size();i++){
                if(text == allDelivery[i]->getAddressee())
                    showList.push_back(i);
                }
        }
    }
    //如果无结果则报错
    if(showList.size()==0){
        QMessageBox::critical(this,"Error",
                              "No Result");
    }
    //显示对应快递
    else
        showDelivery(ui->tableWidget,showList);
}
