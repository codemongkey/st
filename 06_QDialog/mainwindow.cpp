#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QtDebug>
#include <QMessageBox>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenuBar *mBar = menuBar();
    setMenuBar(mBar);
    QMenu *menu  =mBar->addMenu("对话框");

    QAction *p1 = menu->addAction("模态对话框");

    connect(p1,&QAction::triggered,
            [=]()
            {
              QDialog dlg;
              dlg.exec();
              qDebug() <<"1111111111";
            }
            );
    QAction *p2 = menu->addAction("非模态对话框");

    connect(p2,&QAction::triggered,
            [=]()
            {
              QDialog *dlg = new QDialog();
              dlg->setAttribute(Qt::WA_DeleteOnClose);
              dlg->show();
              qDebug() <<"1111111111";
            }
            );
    QAction *p3 = menu->addAction("关于对话框");

    connect(p3,&QAction::triggered,
            [=]()
            {
              QMessageBox::about(this,"about","关于qt");
            }
            );
    QAction *p4 = menu->addAction("问题对话框");

    connect(p4,&QAction::triggered,
            [=]()
            {
              int ret = QMessageBox::question(this,"question","Are you ok?",
                                              QMessageBox::Ok|QMessageBox::Cancel);
              switch(ret)
              {
              case QMessageBox::Yes:
                  qDebug() <<"Yes";
                  break;
              case QMessageBox::No:
                  qDebug() <<"No";
                  break;
              case QMessageBox::Cancel:
                  qDebug() <<"Cancel";
                  break;
              case QMessageBox::Ok:
                  qDebug() <<"Ok";
                  break;
              default:
                  break;
              }
            }
            );


    QAction *p5 = menu->addAction("文件对话框");

    connect(p5,&QAction::triggered,
            [=]()
            {
              QString path = QFileDialog::getOpenFileName(
                          this,
                          "open",
                          "../",
                          tr("Images (*.png *.xpm *.jpg);;"
                             "source (*.cpp *.h *.o);;"
                             "all (*.*)"
                             ));
              qDebug() <<path;
            }
            );

}

MainWindow::~MainWindow()
{

}
