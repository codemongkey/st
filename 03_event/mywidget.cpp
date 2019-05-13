#include "mywidget.h"
#include "ui_mywidget.h"
#include <QDebug>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QTime>
#include <QMessageBox>
#include <QEvent>

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    timerID = this->startTimer(1000);

    connect(ui->pushButton,&MyButton::clicked,
            [=]()
            {
                qDebug() << "按钮被按下";
            }
            );
    //安装过滤器
    ui->label_2->installEventFilter(this);
    ui->label_2->setMouseTracking(true);
}

MyWidget::~MyWidget()
{
    delete ui;
}

void MyWidget::keyPressEvent(QKeyEvent *e)
{
    qDebug() << (char)e->key();

}

void MyWidget::timerEvent(QTimerEvent *e)
{
    if(e->timerId()==this->timerID)
    {
        static int sec = 0;
        ui->label->setText(QString("<center><h1>timer"
                                   " out :%1</h1></center>").arg(sec++));


        if(sec==5){
            killTimer(timerID);
        }
    }
}

void MyWidget::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "++++++++++";
}

void MyWidget::closeEvent(QCloseEvent *e)
{
    int ret = QMessageBox::question(this,"question","是否要关闭");
    if (ret == QMessageBox::Yes) {
        //处理关闭事件，接收事件，事件就不会往下传递
        e->accept();
    }
    else {
        //不关闭窗口
        //忽略事件，事件继续给父组件传递
        e->ignore();
    }
}
bool MyWidget::event(QEvent *e)
{
//    //事件分发
//    switch( e->type() )
//    {
//        case QEvent::Close:
//             this->closeEvent((QCloseEvent *)e);
//                break;


//    }
    if(e->type() == QEvent::Timer){
        //return true;
        return QWidget::event(e);
    }
    else {

        return QWidget::event(e);
    }

}

bool MyWidget::eventFilter(QObject* obj,QEvent* e)
{
    if(obj==ui->label_2){
        QMouseEvent *env = dynamic_cast<QMouseEvent *>(e);
        if(e->type() == QEvent::MouseMove){
            ui->label_2->setText(QString("MOuse move:(%1,%2)").arg(env->x()).arg(env->y()));
            return true;
        }
        else {
            return QWidget::eventFilter(obj,e);
        }
    } else {
        return QWidget::eventFilter(obj,e);
    }
}


