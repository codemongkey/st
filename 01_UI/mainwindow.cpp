#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QCompleter>
#include <QStringList>
#include <QMovie>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->button->setText("123");

    QString str = ui->lineEdit->text();

    qDebug() <<str;

    ui->lineEdit->setText("123456");

    ui->lineEdit->setTextMargins(15,0,0,0);

    //ui->lineEdit->setEchoMode(QLineEdit::Password);

    QStringList list;

    list << "hello" << "how are u";

    QCompleter *com = new QCompleter(list,this);

    com->setCaseSensitivity(Qt::CaseInsensitive);

    ui->lineEdit->setCompleter(com);

    ui->labelText->setText("^_^");

    ui->labelImage->setPixmap(QPixmap("://Image/timg (2).jpeg"));

    ui->labelImage->setScaledContents(true);

    QMovie *mymovie = new QMovie("://Image/singer.gif");

    ui->labelGif->setMovie(mymovie);

    mymovie->start();

    ui->labelGif->setScaledContents(true);

    ui->labelUrl->setText("<h1> <a href=\"https://www.baidu.com\">百度一下</a> </h1>");

    ui->labelUrl->setOpenExternalLinks(true);

    ui->lcdNumber->display(123);

    ui->progressBar->setMaximum(0);
    ui->progressBar->setMaximum(200);
    ui->progressBar->setValue(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_change_clicked()
{
    static int i=0;
    ui->stackedWidget->setCurrentIndex(++i %4);
}
