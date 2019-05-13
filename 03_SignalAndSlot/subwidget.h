#ifndef SUBWIDGET_H
#define SUBWIDGET_H

#include <QWidget>
#include <QPushButton>
class SubWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubWidget(QWidget *parent = nullptr);

signals:
    /*
    * 信号必须使用signals关键字
    * 信号是函数的声明，无需定义
    * 没有返回值，可以有参数，可以重载
    * emit mySignal();
    *
    */
    void mySignal();
    void mySignal(int,QString);




public slots:
    void sendSlot();
private:
    QPushButton b;
};

#endif // SUBWIDGET_H
