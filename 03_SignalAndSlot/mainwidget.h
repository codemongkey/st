#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "subwidget.h"
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();

    void mySLot();
    void changeWin();
    void dealSub();
    void dealSlot(int a,QString str);
private:
    QPushButton b1;
    QPushButton *b2;
    QPushButton b3;

    SubWidget w;
};

#endif // MAINWIDGET_H
