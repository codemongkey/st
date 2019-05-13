#include <QApplication>
#include <QWidget>
#include <QPushButton>
int main(int argc, char** argv){

    QApplication a(argc,argv);

    QWidget w;

    w.setWindowTitle(QString("主要看气质"));



    QPushButton b;

    b.setText("^_^");

    b.setParent(&w);
    b.move(100,100);

    QPushButton b1(&w);
    b1.setText("abc");

    w.show();

    a.exec();

    return 0;
}
