#include "pstmpage.h"
#include "ui_pstmpage.h"

pstmpage::pstmpage(QString username, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pstmpage)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    _postmanName = username;
    init();
}

pstmpage::~pstmpage()
{
    save();
    for(int i=0;i<_allDelivery.size();i++)
        delete _allDelivery[i];
    delete ui;
}

void pstmpage::init()
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
        if(type == 0) {
            _adminBalance = balance;
        }else if(_postmanName == username){
            _postmanBalance = balance;
            _postman = user(balance,username,name,loc,tele);
        }
    }
    inFile.close();

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
        qDebug() << postman << endl;
        mytime st(smonth,sday,shour,smin),rt(rmonth,rday,rhour,rmin);
        delivery* tmp;
        if(postman == _postmanName){
            if(type == 1)
                tmp = new book(id,n,status,sender,addressee,postman,st,desc,rt);
            else if(type == 2)
                tmp = new fragile(id,n,status,sender,addressee,postman,st,desc,rt);
            else if(type == 3)
                tmp = new normal(id,n,status,sender,addressee,postman,st,desc,rt);
            if(status == delivery::toBeCollected) _showList.push_back(_allDelivery.size());
            _allDelivery.push_back(tmp);
        }

    }
    deliFile.close();
    ui->tableWidget->setColumnCount(8);
    showDelivery();
    //设置tableWidget选中模式为整行选中并且不允许编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置表头标题
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Description"
        << "Type" << "Number" << "Sender" << "Recipient"
        << "Postman" << "Send Time"<<"Receive Time");
    //当点击单元格时更新列表信息为当前信息
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&pstmpage::flushList);
    //当单元格信息发生更改时更新列表信息
    connect(ui->tableWidget,&QTableWidget::cellChanged,this,&pstmpage::flushList);
    connect(ui->collectbtn,&QPushButton::clicked,this,&pstmpage::collectPkg);
    connect(this,&pstmpage::sendQuery,this,&pstmpage::queryPkg);
    connect(ui->querybtn,&QPushButton::clicked,this,&pstmpage::querypage);
    connect(ui->quitbtn,&QPushButton::clicked,this,[=](){
        this->close();
        this->parentWidget()->show();
    });
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,[&](int row){
        QString tmp = ui->tableWidget->item(row,7)->text();
        _curRow = row;
        if(tmp == "Not yet collected") ui->collectbtn->setEnabled(true);
        else ui->collectbtn->setEnabled(false);
    });

    //配置用户详情页
    _detail = new SlidePage(5,"UserDetail",ui->widget);
    QLabel* userLabel = new QLabel(_detail);
    userLabel->setText("Username:"+_postman.getUserName());
    userLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QLabel* nameLabel = new QLabel(_detail);
    nameLabel->setText("Name:"+_postman.getName());
    nameLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QLabel* locLabel = new QLabel(_detail);
    locLabel->setText("Loction:"+_postman.getLocation());
    locLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QLabel* teleLabel = new QLabel(_detail);
    teleLabel->setText("Tele.:"+_postman.getTelephone());
    teleLabel->setFont(QFont("Corbel",12,QFont::Normal));
    QLabel* banLabel = new QLabel(_detail);
    banLabel->setText("Balance:"+QString::number(_postman.getBalance()));
    banLabel->setFont(QFont("Corbel",12,QFont::Normal));

    _detail->AddContent(userLabel);
    _detail->AddContent(nameLabel);
    _detail->AddContent(locLabel);
    _detail->AddContent(teleLabel);
    _detail->AddContent(banLabel);

    connect(ui->collectbtn,&QPushButton::clicked,this,[=](){
        banLabel->setText("Balance:"+QString::number(_postmanBalance));
    });
    connect(ui->detailbtn,&QPushButton::clicked,_detail,&SlidePage::slideIn);
}

void pstmpage::showDelivery()
{
    QTableWidget* table = ui->tableWidget;
    //要先断开单元格内容变更同更新列表的连接，否则会导致程序崩溃
    disconnect(table,&QTableWidget::cellChanged,this,&pstmpage::flushList);

    //清空原有表格内容
    for(int row = table->rowCount() - 1;row >= 0; row--){
        table->removeRow(row);
    }

    //按表格形式显示快递
    int curRow = 0;
    for(int i=0;i<_showList.size();i++){
        int idx = _showList[i];
        table->insertRow(curRow);
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
    connect(table,&QTableWidget::cellChanged,this,&pstmpage::flushList);
}

void pstmpage::querypage()
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
        QString("format:Month.Date\nIf you query by sender/recipient,please input his/her username,like sender:user1\n") +
        QString("If you query by courier number please input an interger\nif you input nothing,then all ") +
        QString("deliveries which havn't been collected will be shown."));
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

void pstmpage::queryPkg(QString text)
{
    bool isDate = false;
    bool isNumber = false;
    int courierN = text.toInt(&isNumber);
    //清空展示快递的数组
    _showList.clear();
    //只有查询日期时才会出现.字符，以此判断是否查询日期
    for(int i=0;i<text.size();i++){
        if(text[i]=='.') {isDate = true;break;}
    }
    //如果输入为空，则全部显示
    if(text.isEmpty()){
        for(int i=0;i<_allDelivery.size();i++)
           if(_allDelivery[i]->getStatus() == delivery::toBeCollected)
                _showList.push_back(i);
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
        for(int i=0;i< _allDelivery.size();i++){
            if(courierN == _allDelivery[i]->getid()) {_showList.push_back(i); break;}
        }
    }
    //剩下只可能输入用户名，则查找关联快递
    else{
        if(text.startsWith("sender:")){
            text = text.mid(7);
            for(int i=0;i<_allDelivery.size();i++){
                if(text == _allDelivery[i]->getSender())
                    _showList.push_back(i);
                }
        }
        else if(text.startsWith("recipient:")){
            text = text.mid(10);
            for(int i=0;i<_allDelivery.size();i++){
                if(text == _allDelivery[i]->getAddressee())
                    _showList.push_back(i);
                }
        }
    }
    //如果无结果则报错
    if(_showList.size()==0 && !text.isEmpty()){
        QMessageBox::critical(this,"Error",
                              "No Result");
    }
    //显示对应快递
    else
        showDelivery();
}

void pstmpage::flushList(int row)
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

void pstmpage::collectPkg()
{
    int idx = _showList[_curRow];
    _allDelivery[idx]->setNeedRewrite();
    _allDelivery[idx]->setStatus(delivery::toBeSigned);
    _postmanBalance += _allDelivery[idx]->getPrice() / 2;
    _adminBalance -= _allDelivery[idx]->getPrice() / 2;
    ui->tableWidget->hideRow(_curRow);
}

void pstmpage::save()
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
        fout << username << endl;
        if(username == "admin") fout << _adminBalance <<" "<< type << endl;
        else if(username == _postmanName) fout << _postmanBalance <<" "<< type << endl;
        else fout << balance <<" "<< type << endl;
        fout << pwd << endl;
        fout << name << endl;
        fout << tele << endl;
        fout << loc << endl;
    }
    inFile.close();
    outFile.close();
    inFile.remove();
    outFile.rename(USERPATH);
}
