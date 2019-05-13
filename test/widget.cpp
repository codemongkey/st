#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QPushButton *button = new QPushButton("^_^",this);
    button->setGeometry(100,100,200,200);

    connect(button,&QPushButton::clicked,
            [](){});
}

Widget::~Widget()
{
    delete ui;
}
