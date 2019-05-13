#include "mybutton.h"

MyButton::MyButton(QWidget *parent) : QPushButton(parent)//通过构造函数给基类传参
{

}

MyButton::~MyButton()
{
    qDebug() << "按钮被析构";
}
