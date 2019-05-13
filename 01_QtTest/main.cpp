#include "mywidget.h"
#include <QApplication>
//QApplication应用程序类
//Qt头文件没有.h
//

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    MyWidget w;

    w.show();

    return a.exec();
}
