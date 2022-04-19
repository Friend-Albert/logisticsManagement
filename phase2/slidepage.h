#ifndef SLIDEPAGE_H
#define SLIDEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QGraphicsEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
class SlidePage : public QWidget
{
    Q_OBJECT
private:
    int preferWidth = 350;
    int cornerRadius;
    QString pageName;
    QLabel *nameLabel;
    QFont textFont = QFont("Corbel Light", 24);

    QParallelAnimationGroup *curAni = nullptr;
    QGraphicsOpacityEffect *opacity;
    QVBoxLayout *mainLayout;

public:
    explicit SlidePage(int radius, QString name, QWidget *parent = nullptr);
    void SetRadius(int radius);
    void SetName(QString name);
    void AddContent(QWidget* widget);

signals:

public slots:
    void slideIn();
    void slideOut();

};

#endif // SLIDEPAGE_H
