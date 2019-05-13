#include "mybutton.h"
#include <QMouseEvent>
#include <QDebug>
MyButton::MyButton(QWidget *parent) : QPushButton(parent)
{

}
void MyButton::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() ==Qt::LeftButton)
    {
        qDebug() << "按下的是左键";
        //ev->ignore();
        //事件接收后不会往下传递
    }
    else
    {
        QPushButton::mousePressEvent(ev);
    }
}
