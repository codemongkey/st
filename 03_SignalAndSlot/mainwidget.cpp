#include "mainwidget.h"
#include <QPushButton>
#include <QDebug>
MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    b1.setParent(this);
    b1.setText("close");
    b1.move(100,100);

    b2 = new QPushButton(this);
    b2->setText("123");

    connect(&b1, &QPushButton::pressed,this,&MainWidget::close);

    connect(b2, &QPushButton::released,this,&MainWidget::mySLot);

    connect(b2, &QPushButton::released,&b1,&QPushButton::hide);

    setWindowTitle("老大");

    b3.setParent(this);
    b3.setText("切换到子窗口");
    b3.move(50,50);

    connect(&b3,&QPushButton::pressed,this,&MainWidget::changeWin);

    void(SubWidget::*funSignal)() = &SubWidget::mySignal;

    void(SubWidget::*testSignal)(int,QString) = &SubWidget::mySignal;

    connect(&w,funSignal,this,&MainWidget::dealSub);

    connect(&w,testSignal,this,&MainWidget::dealSlot);

    QPushButton *b4=new QPushButton(this);
    b4->setText("Lambda表达式");
    b4->move(150,150);
    int a=10,b=10;
    connect(b4,&QPushButton::clicked,
            // = 外部所有局部变量以及类中所有成员 值传递
            // this: 类中所有变量值传递
            // &: 切记槽函数的声明周期要比构造函数的生命周期要长 &的时候要注意
            [=](bool isCheck) mutable
            {
                qDebug() << isCheck;
            }
            );




    resize(400,300);
    //w.show();
}

void MainWidget::mySLot()
{
    b2->setText("abc");
}
void MainWidget::dealSlot(int a,QString str)
{
    qDebug() <<a <<str.toUtf8().data();
}
void MainWidget::dealSub()
{
    w.hide();
    this->show();
}
void MainWidget::changeWin()
{
    w.show();
    this->hide();
}
MainWidget::~MainWidget()
{
    delete  b2;
}
