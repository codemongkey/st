#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QCloseEvent>
namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = nullptr);
    ~MyWidget();
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void timerEvent(QTimerEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void closeEvent(QCloseEvent *e) override;

    bool event(QEvent *e) override;

    bool eventFilter(QObject* obj,QEvent* e) override;
private:
    Ui::MyWidget *ui;

    int timerID;
};

#endif // MYWIDGET_H
