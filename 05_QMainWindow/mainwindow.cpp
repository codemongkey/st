#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QDebug>
#include <QToolBar>
#include <QPushButton>
#include <QStatusBar>
#include <QLabel>
#include <QTextEdit>
#include <QDockWidget>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(600,400);
    move(600,200);
    //菜单栏
    QMenuBar *mBar = menuBar();

    //添加菜单
    QMenu *pFile = mBar->addMenu("文件");

    //添加菜单项
    QAction *pNew = pFile->addAction("新建");

    connect(pNew, &QAction::triggered,
            [=]()
            {
              qDebug() <<"新建被按下";
            }
            );
    //添加分割线
    pFile->addSeparator();

    QAction *pOpen = pFile->addAction("打开");
    //工具栏
    QToolBar *toolBar = addToolBar("toolBar");

    toolBar->addAction(pNew);
    //添加小控件
    QPushButton *b = new QPushButton(this);
    b->setText("^_^");
    toolBar->addWidget(b);

    connect(b,&QPushButton::clicked,
            [=]()
            {
              b->setText("123");
            });

    //状态栏
    QStatusBar *sBar = statusBar();

    QLabel *label = new QLabel(this);
    label->setText("Normal text file");
    sBar->addWidget(label);
    // 从左右添加
    sBar->addWidget(new QLabel("2",this));
    sBar->addPermanentWidget(new QLabel("2",this));
    //核心控件
    QTextEdit *textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);



    //浮动窗口
    QDockWidget *dock = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea,dock);
    QTextEdit *textEdit1 = new QTextEdit(this);
    dock->setWidget(textEdit1);

}

MainWindow::~MainWindow()
{

}
