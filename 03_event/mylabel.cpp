#include "mylabel.h"
#include <QMouseEvent>
MyLabel::MyLabel(QWidget *parent) : QLabel(parent)
{
    this->setMouseTracking(true);
}
void MyLabel::mousePressEvent(QMouseEvent *ev)
{
    int i = ev->x();
    int j = ev->y();
    QString str = QString("<center><h1>Mouse Press: (%1,%2)</h1></center>")
            .arg(i).arg(j);
    this->setText(str);
};

void MyLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    int i = ev->x();
    int j = ev->y();
    QString str = QString("<center><h1>Mouse Release: (%1,%2)</h1></center>")
            .arg(i).arg(j);
    this->setText(str);

};

void MyLabel::mouseMoveEvent(QMouseEvent *ev)
{
    int i = ev->x();
    int j = ev->y();
    QString str = QString("<center><h1>Mouse Move: (%1,%2)</h1></center>")
            .arg(i).arg(j);
    this->setText(str);

};
void MyLabel::enterEvent(QEvent *e)
{
    QString str = QString("<center><h1>Mouse Enter</h1></center>");
    this->setText(str);
}
void MyLabel::leaveEvent(QEvent *e)
{
    QString str = QString("<center><h1>Mouse leave</h1></center>");
    this->setText(str);
}
