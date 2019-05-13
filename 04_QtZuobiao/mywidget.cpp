#include "mywidget.h"
#include "mybutton.h"
#include <QPushButton>

MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
{   
    move(1000,360);

    QPushButton *b1 = new QPushButton(this);

    b1->move(100,100);
    b1->setText("@_@");
    b1->resize(200,100);

    QPushButton *b2 = new QPushButton(b1);
    b2->move(10,10);
    b2->setText("@_@");

    MyButton* b3 = new MyButton(this);
    b3->setText("123");
    // 1)指定父对象后 2)直接继承与QObject 内存回收机制
    //
}

MyWidget::~MyWidget()
{

}
