#include "userpage.h"
#include "ui_mainwindow.h"
#include "basic.h"
/**
 * @brief 信号和槽的连接以及ui相关设置
 * @param parent 父窗口指针
 */
userpage::userpage(QString username,QString pwd,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("logisticsManagement");
    this->setAttribute(Qt::WA_DeleteOnClose);
    _username = username;
    _password = pwd;

    //设置tableWidget列数
    ui->tableWidget->setColumnCount(tableColumns);

    sendbox = new sendpage(this);

    //初始化
    init();
    //点击签收按钮后更新表格中信息
    connect(ui->signbtn,&QPushButton::clicked,this,&userpage::signPkg);
    ui->sendbtn->setText("Send Package");
    //点击查询按钮显示查询页面
    connect(ui->querybtn,&QPushButton::clicked,this,&userpage::queryPage);
    //填写寄件信息完毕后返回错误信息或添加新快递信息
    connect(sendbox,&sendpage::trySend,this,&userpage::sendPkg);
    //发出查询请求时进行查询
    connect(this,&userpage::sendQuery,this,&userpage::queryPkg);
    //退出时关闭主界面打开登录界面
    connect(ui->quitbtn,&QPushButton::clicked,this,[=](){
        this->parentWidget()->show();
        delete this;
    });
}

/**
 * @brief 保存信息并释放登录，寄件和本页面
 */
userpage::~userpage()
{
    save();
    for(int i=0;i<_allDelivery.size();i++)
        delete _allDelivery[i];
    if(detail) delete detail;
    if(detail) delete sendbox;
    delete ui;
}

/**
 * @brief 获得登录用户的昵称
 * @return 用户昵称字符串
 */
const QString &userpage::username() const
{
    return _username;
}

/**
 * @brief 设置登录用户的用户名
 * @param newUsername 登录用户的用户名
 */
void userpage::setUsername(const QString &newUsername)
{
    _username = newUsername;
}

/**
 * @brief 读取登录用户的所需信息并设置ui样式
 */
void userpage::init()
{
    //获得当前用户详细信息
    QFile userFile(USERPATH);
    userFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream fin(&userFile);
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
        qDebug()<<username<<endl;
        _allUsername.insert(username);
        if(username == _username){
            curUser = user(balance,username,name,loc,tele);
        }
        if(username == "admin"){
            admin = user(balance,username,name,loc,tele);
        }
    }
    userFile.close();

    //获得当前用户的所有关联快递
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
        qDebug() << id << endl;
        //如果是管理员或者寄件人收件人一方是当前用户
        if(sender == _username || addressee == _username){
            mytime st(smonth,sday,shour,smin),rt(rmonth,rday,rhour,rmin);
            delivery* tmp;
            if(type == 1)
                tmp = new book(id,n,status,sender,addressee,postman,st,desc,rt);
            else if(type == 2)
                tmp = new fragile(id,n,status,sender,addressee,postman,st,desc,rt);
            else if(type == 3)
                tmp = new normal(id,n,status,sender,addressee,postman,st,desc,rt);
            _allDelivery.push_back(tmp);
            _showList.push_back(_allDelivery.size()-1);
        }
    }
    deliFile.close();


    ui->label->setText("Delivery(User:"+_username+")");

    //显示所有关联快递
    showDelivery(ui->tableWidget,_showList);

    //设置tableWidget选中模式为整行选中并且不允许编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置表头标题
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Description"
        << "Type" << "Number" << "Sender" << "Recipient"
        << "Postman" << "Send Time"<<"Receive Time");

    //配置用户详情页
    detail = new SlidePage(5,"UserDetail",ui->widget);
    QLabel* userLabel = new QLabel(detail);
    userLabel->setText("Username:"+curUser.getUserName());
    userLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QLabel* nameLabel = new QLabel(detail);
    nameLabel->setText("Name:"+curUser.getName());
    nameLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QLabel* locLabel = new QLabel(detail);
    locLabel->setText("Loction:"+curUser.getLocation());
    locLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QLabel* teleLabel = new QLabel(detail);
    teleLabel->setText("Tele.:"+curUser.getTelephone());
    teleLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QLabel* banLabel = new QLabel(detail);
    banLabel->setText("Balance:"+QString::number(curUser.getBalance()));
    banLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QPushButton* changebtn = new QPushButton(detail);
    changebtn->setText("Change Password");
    changebtn->setFont(QFont("Corbel",12,QFont::Normal));
    QPushButton* chargebtn = new QPushButton(detail);
    chargebtn->setText("Recharge");
    chargebtn->setFont(QFont("Corbel",12,QFont::Normal));

    detail->AddContent(userLabel);
    detail->AddContent(nameLabel);
    detail->AddContent(locLabel);
    detail->AddContent(teleLabel);
    detail->AddContent(banLabel);
    detail->AddContent(changebtn);
    detail->AddContent(chargebtn);

    disconnect(this,&userpage::tryRecharge,0,0);
    connect(chargebtn,&QPushButton::clicked,this,[this,banLabel](){emit tryRecharge(banLabel);});
    connect(this,&userpage::tryRecharge,this,&userpage::balance);
    connect(changebtn,&QPushButton::clicked,this,&userpage::changePwd);

    connect(ui->detailbtn,&QPushButton::clicked,detail,&SlidePage::slideIn);
    connect(ui->detailbtn,&QPushButton::clicked,detail,[=](){
        banLabel->setText("Balance:"+QString::number(curUser.getBalance()));
    });
    //当点击单元格时更新列表信息为当前信息
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&userpage::flushList);
    //当单元格信息发生更改时更新列表信息
    connect(ui->tableWidget,&QTableWidget::cellChanged,this,&userpage::flushList);
    //当点击单元格时判断选中快递是否能签收
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&userpage::needSigned);
    //当点击单元格时设置curRow的值
    //connect(ui->tableWidget,&QTableWidget::cellClicked,this,[&](int row){curRow = row;});
    ui->sendbtn->setText("Send Package");
    //点击寄件按钮后显示寄件页面
    connect(ui->sendbtn,&QPushButton::clicked,sendbox,[&](){sendbox->show();});
}

/**
 * @brief 保存快递和用户信息
 * 思路：将除目标信息外，其它信息拷贝到一个临时文件中，然后删除原来的文件，并将临时文件的文件名改为和原文件相同的名称。
 */
void userpage::save()
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
    for(int i=0;i<_allDelivery.size();i++){
        if(_allDelivery[i]->getNeedRewrite()){
            delivery& dly = *_allDelivery[i];
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

    //写入用户信息的思路同上
    QString username,name,tele,loc,pwd;
    int balance;
    inFile.setFileName(USERPATH);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    fin.setDevice(&inFile);
    outFile.setFileName(TEMPPATH);
    outFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate);
    fout.setDevice(&outFile);
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
        //当读入到当前用户信息或者管理员信息时，输入内存中的信息
        if(username == curUser.getUserName() ){
            fout << curUser.getUserName() << endl;
            fout << curUser.getBalance() <<" "<< type << endl;
            fout << pwd << endl;
            fout << curUser.getName() << endl;
            fout << curUser.getTelephone() << endl;
            fout << curUser.getLocation() << endl;
        }else if(username == "admin"){
            fout << admin.getUserName() << endl;
            fout << admin.getBalance() <<" "<< type << endl;
            fout << pwd << endl;
            fout << admin.getName() << endl;
            fout << admin.getTelephone() << endl;
            fout << admin.getLocation() << endl;
        }
        //否则原样拷贝
        else{
            fout << username << endl;
            fout << balance <<" "<< type << endl;
            fout << pwd << endl;
            fout << name << endl;
            fout << tele << endl;
            fout << loc << endl;
        }
    }
    inFile.close();
    outFile.close();
    inFile.remove();
    outFile.rename(USERPATH);
}

/**
 * @brief 依照索引显示对应快递
 * @param table 需要显示快递信息的tableWidget指针
 * @param all 需要显示快递的索引
 */
void userpage::showDelivery(QTableWidget *table, const QVector<int> &all)
{
    //要先断开单元格内容变更同更新列表的连接，否则会导致程序崩溃
    disconnect(ui->tableWidget,&QTableWidget::cellChanged,this,&userpage::flushList);

    //清空原有表格内容
    for(int row = table->rowCount() - 1;row >= 0; row--){
        table->removeRow(row);
    }

    //按表格形式显示快递
    curRow = 0;
    for(int i=0;i<all.size();i++){
        int idx = all[i];
        table->insertRow(curRow);
        _rowId[curRow] = _allDelivery[idx]->getid();
        delivery& d = *_allDelivery[idx];
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
    connect(ui->tableWidget,&QTableWidget::cellChanged,this,&userpage::flushList);
}

/**
 * @brief 刷新列表的快递信息
 * @param row 新选中快递的行数
 */
void userpage::flushList(int row)
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
 * @brief 查看选中行快递是否能签收
 * @param row 选中快递的行数
 */
void userpage::needSigned(int row)
{
    QTableWidget* table = ui->tableWidget;
    QString addressee = table->item(row,2)->text();
    QString status = table->item(row,7)->text();
    //如果能签收，则设置签收按钮可以点击
    if(addressee==_username&&status.contains("Signed")){
        ui->signbtn->setEnabled(true);
    }
    else
        ui->signbtn->setEnabled(false);
}

/**
 * @brief 签收快递
 */
void userpage::signPkg()
{
    mytime curTime;
    curTime.getRealTime();
    //找到该行快递的快递单号，设置其已签收并需要重写
    for(int i=0;i<_allDelivery.size();i++){
        if(_allDelivery[i]->getid() == _rowId[curRow]){
            _allDelivery[i]->setNeedRewrite();
            _allDelivery[i]->setSigned();
        }
    }
    //更新表格中的信息
    ui->tableWidget->item(curRow,4)->setText(curTime.toString());
    //禁用签收按钮
    ui->signbtn->setEnabled(false);
}

/**
 * @brief 处理寄件时的不同情况
 * @param addressee 收件人
 * @param desc 快递描述
 */
void userpage::sendPkg(QString addressee, QString desc,int number,int type)
{
    delivery* tmp;
    mytime curTime;
    curTime.getRealTime();
    if(type == 1)
        tmp = new book(++nextId,number,delivery::toBeCollected,
                       curUser.getUserName(),addressee,QString("None"),curTime,desc);
    else if(type == 2)
        tmp = new fragile(++nextId,number,delivery::toBeCollected,
                          curUser.getUserName(),addressee,QString("None"),curTime,desc);
    else
        tmp = new normal(++nextId,number,delivery::toBeCollected,
                         curUser.getUserName(),addressee,QString("None"),curTime,desc);
    qDebug() << tmp->getid()<<endl;
    //余额不足则给出提示
    if(curUser.getBalance()< tmp->getPrice()){
        QMessageBox::critical(sendbox,"Error","Sorry, your credit is running low.");
        delete tmp;
    }
    //如果收件人正确则新建快递并更改用户余额
    else if(addressee!=curUser.getUserName()&&_allUsername.find(addressee)!=_allUsername.end()){
        tmp->setNeedRewrite();
        _allDelivery.push_back(tmp);
        _showList.push_back(_allDelivery.size()-1);
        curUser.setBalance(tmp->getPrice());
        admin.recharge(tmp->getPrice());
        sendbox->close();
        showDelivery(ui->tableWidget,_showList);
    }
    //否则提示输入有效的收件人
    else{
        QMessageBox::critical(sendbox,"Error","please input vaild recipient!");
    }
}

/**
 * @brief 显示更改密码页面
 */
void userpage::changePwd()
{
    QDialog *change = new QDialog(this);
    change->setAttribute(Qt::WA_DeleteOnClose);
    change->resize(720,480);
    QVBoxLayout* mainLayout = new QVBoxLayout(change);
    QFormLayout* layout = new QFormLayout();
    QHBoxLayout* btn = new QHBoxLayout();
    QLineEdit* old = new QLineEdit();
    old->setEchoMode(QLineEdit::Password);
    QLineEdit* newpwd = new QLineEdit();
    newpwd->setEchoMode(QLineEdit::Password);
    QLineEdit* confirm = new QLineEdit();
    confirm->setEchoMode(QLineEdit::Password);
    layout->addRow("Old Password:",old);
    layout->addRow("New Password:",newpwd);
    layout->addRow("Confirm Password:",confirm);
    QPushButton* okbtn = new QPushButton("OK");
    QPushButton* cancelbtn = new QPushButton("Cancel");
    btn->addWidget(okbtn);
    btn->addWidget(cancelbtn);
    mainLayout->addLayout(layout);
    mainLayout->addLayout(btn);
    change->show();

    connect(cancelbtn,&QPushButton::clicked,change,[=](){change->close();});
    //点击ok按钮后，对比旧密码是否正确，并更改新密码
    connect(okbtn,&QPushButton::clicked,change,[=](){
        QString old1 = old->text(),new1 = newpwd->text(),confirm1 = confirm->text();
        //如果新密码过短
        if(new1.size()<6){
            QMessageBox::critical(this,"Error",
                             "Your password is too short(least 6 characters)");
            newpwd->clear();
            confirm->clear();
        }
        //如果两次输入新密码不一致
        else if(new1 != confirm1){
            QMessageBox::critical(this,"Error",
                                  "The two passwords are inconsistent!");
            newpwd->clear();
            confirm->clear();
        }
        //如果旧密码不一致
        else if(old1 != _password){
            QMessageBox::critical(this,"Error",
                                  "The old password is incorrect");
            old->clear();
        }
        //将密码更改同步到登录页面下存储的用户密码表中
        else{
            emit newPwd(curUser.getUserName(),new1);
            _password = new1;
            change->close();
        }
    });
}

/**
 * @brief 充值页面
 * @param _balance：侧边栏中显示余额的标签，用于同步更改
 */
void userpage::balance(QLabel* _balance)
{
    QDialog *change = new QDialog(this);
    change->setAttribute(Qt::WA_DeleteOnClose);
    change->resize(240,160);
    QVBoxLayout* mainLayout = new QVBoxLayout(change);
    QFormLayout* flayout = new QFormLayout();
    QHBoxLayout* btn = new QHBoxLayout();
    QLineEdit* charge = new QLineEdit();
    flayout->addRow("Recharge:",charge);
    QPushButton* okbtn = new QPushButton("OK");
    QPushButton* cancelbtn = new QPushButton("Cancel");
    btn->addWidget(okbtn);
    btn->addWidget(cancelbtn);
    mainLayout->addLayout(flayout);
    mainLayout->addLayout(btn);
    change->show();

    //点击cancel按钮关闭窗口
    connect(cancelbtn,&QPushButton::clicked,change,[=](){change->close();});
    //点击OK按钮检查输入合法性并更改余额
    connect(okbtn,&QPushButton::clicked,change,[=](){
        QString pay = charge->text();
        bool valid = true;
        int payment = pay.toInt(&valid);
        //如果输入为整数且为正数
        if(valid && payment > 0){
            curUser.recharge(payment);
            _balance->setText("Balance:"+QString::number(curUser.getBalance()));
            change->close();
        }else{
            charge->clear();
            QMessageBox::critical(detail,"Error",
            "Please input valid number(must be an int and greater than 0)");
        }
    });
}

/**
 * @brief 显示查询快递页面
 */
void userpage::queryPage()
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
        QString("format:Month.Date\nIf you query by sender/recipient,please input his/her username\n") +
        QString("If you query by courier number please input an interger\nif you input nothing,then all ") +
        QString("deliveries will be shown.\nIf you query by state,please input:toBeCollected\\") +
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

/**
 * @brief 依照不同的查询文本格式给出对应结果
 * @param 查询输入的文本
 */
void userpage::queryPkg(QString text)
{
    int isStatus = 0;
    bool isDate = false;
    bool isNumber = false;
    int courierN = text.toInt(&isNumber);
    //清空展示快递的数组
    _showList.clear();
    //只有查询日期时才会出现.字符，以此判断是否查询日期
    for(int i=0;i<text.size();i++){
        if(text[i]=='.') {isDate = true;break;}
    }
    //判断是否在查询物品状态
    if(text == "toBeCollected")
        isStatus = 0x00;
    else if( text == "toBeSigned")
        isStatus = 0x01;
    else if(text == "signed")
        isStatus = 0x10;

    //如果输入为空，则全部显示
    if(text.isEmpty()){
        for(int i=0;i<_allDelivery.size();i++) _showList.push_back(i);
    }
    //如果输入日期，则查找对应快递
    else if(isDate){
        for(int i=0;i<_allDelivery.size();i++){
            mytime t1 = _allDelivery[i]->getSendTime();
            mytime t2 = _allDelivery[i]->getRecvTime();
            if(t1.isSameDay(text)||t2.isSameDay(text)){
                _showList.push_back(i);
            }
        }
    }
    //如果输入数字，则查找对应快递单号的快递
    else if(isNumber){
        for(int i=0;i<_allDelivery.size();i++){
            if(courierN == _allDelivery[i]->getid()) {_showList.push_back(i); break;}
        }
    }
    else if(isStatus){
        for(int i=0;i<_allDelivery.size();i++){
            if(_allDelivery[i]->getStatus() == isStatus)
                _showList.push_back(i);
        }
    }
    //剩下只可能输入用户名，则查找关联快递
    else{
        if(text.startsWith("Sender:")){
            text = text.mid(7);
            for(int i=0;i<_allDelivery.size();i++){
                if(text == _allDelivery[i]->getSender())
                    _showList.push_back(i);
                }
        }
        else if(text.startsWith("Recipient:")){
            text = text.mid(10);
            for(int i=0;i<_allDelivery.size();i++){
                if(text == _allDelivery[i]->getAddressee())
                    _showList.push_back(i);
                }
        }
    }
    //如果无结果则报错
    if(_showList.size()==0){
        QMessageBox::critical(this,"Error",
                              "No Result");
    }
    //显示对应快递
    else
        showDelivery(ui->tableWidget,_showList);
}
