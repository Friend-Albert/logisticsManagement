#include "slidepage.h"
#include <QPushButton>

/**
 * @brief 侧边栏界面设置
 * @param radius：窗体圆角程度
 * @param name：标题名称
 * @param parent：父窗口指针
 */
SlidePage::SlidePage(int radius, QString name, QWidget *parent) :
    QWidget(parent),
    cornerRadius(radius),
    pageName(name)
{
    //检查宽度是否合适
    if(parent)
        resize(parent->width() * 0.8 <= preferWidth ? parent->width() * 0.8 : preferWidth, parent->height());
    this->move(QPoint(-this->width() - 30, 0));

    nameLabel = new QLabel(pageName, this);
    textFont.setStyleStrategy(QFont::PreferAntialias);
    nameLabel->setFont(textFont);

    opacity = new QGraphicsOpacityEffect(this);
    opacity->setOpacity(0);
    nameLabel->setGraphicsEffect(opacity);

    QString style;
    style = "background-color:rgba(250,250,250,250);border-radius:20px";
    QWidget *backGround = new QWidget(this);
    backGround->lower();
    backGround->resize(this->size());
    backGround->setStyleSheet(style);
    backGround->show();

    //初始化布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 40, 20, 20);
    QWidget *titleBar = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    ;
    QPushButton* quitbtn = new QPushButton();
    quitbtn->setStyleSheet("QPushButton{background-color:rgba(210,210,210,50%);width:20px;height:20px;border:2px;border-radius: 10px;}"
                            "QPushButton:hover{background-color:#ff6767;color:black;width:20px;height:20px;}");
    titleLayout->setAlignment(Qt::AlignLeft);
    titleBar->setLayout(titleLayout);
    titleLayout->addWidget(quitbtn);
    titleLayout->addWidget(nameLabel);
    mainLayout->addWidget(titleBar);
    mainLayout->setAlignment(Qt::AlignTop);
    this->setLayout(mainLayout);

    //设置窗体阴影
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0));
    shadow->setOffset(0, 0);
    this->setGraphicsEffect(shadow);

    //点击退出按钮时关闭窗体
    connect(quitbtn, &QPushButton::clicked, this, [=](){slideOut();});
}

/**
 * @brief 设置窗体圆角
 * @param radius：圆角程度
 */
void SlidePage::SetRadius(int radius){
    cornerRadius = radius;
    QString style;
    style = "background-color:white;border-radius:" + QString::asprintf("%d", cornerRadius) + "px";
    this->setStyleSheet(style);
}

/**
 * @brief 设置标题栏名称
 * @param name：名称
 */
void SlidePage::SetName(QString name){
    pageName = name;
    nameLabel->setText(pageName);
}
/**
 * @brief 添加组件
 * @param widget：组件指针
 */
void SlidePage::AddContent(QWidget *widget)
{
    mainLayout->addWidget(widget);
}

/**
 * @brief 淡入页面动画
 */
void SlidePage::slideIn(){
    if(curAni){
        curAni->stop();
        curAni->deleteLater();
        curAni = nullptr;
    }
    QParallelAnimationGroup *inGroup = new QParallelAnimationGroup(this);
    //滑动进入动画
    QPropertyAnimation *slideInAni = new QPropertyAnimation(this, "pos", this);
    slideInAni->setStartValue(this->pos());
    slideInAni->setEndValue(QPoint(0, 0));
    slideInAni->setDuration(1000);
    slideInAni->setEasingCurve(QEasingCurve::InOutExpo);
    //窗口透明度变化动画
    QPropertyAnimation *fadeInAni = new QPropertyAnimation(opacity, "opacity", this);
    fadeInAni->setStartValue(opacity->opacity());
    fadeInAni->setEndValue(0.99);
    fadeInAni->setDuration(750);
    //设置二者同时发生
    inGroup->addAnimation(slideInAni);
    inGroup->addAnimation(fadeInAni);
    connect(inGroup, &QParallelAnimationGroup::finished, this, [=](){this->curAni = nullptr;});
    inGroup->start();
    curAni = inGroup;
}

/**
 * @brief 淡出页面动画，思路同淡入一致
 */
void SlidePage::slideOut(){
    if(curAni){
        curAni->stop();
        curAni->deleteLater();
        curAni = nullptr;
    }
    QParallelAnimationGroup *outGroup = new QParallelAnimationGroup(this);
    QPropertyAnimation *slideOutAni = new QPropertyAnimation(this, "pos", this);
    slideOutAni->setStartValue(this->pos());
    slideOutAni->setEndValue(QPoint(-this->width() - 30, 0));
    slideOutAni->setDuration(1000);
    slideOutAni->setEasingCurve(QEasingCurve::InOutExpo);
    QPropertyAnimation *fadeOutAni = new QPropertyAnimation(opacity, "opacity", this);
    fadeOutAni->setStartValue(opacity->opacity());
    fadeOutAni->setEndValue(0);
    fadeOutAni->setDuration(750);
    outGroup->addAnimation(slideOutAni);
    outGroup->addAnimation(fadeOutAni);
    connect(outGroup, &QPropertyAnimation::finished, this, [=](){this->curAni = nullptr;});
    outGroup->start();
    curAni = outGroup;
}
