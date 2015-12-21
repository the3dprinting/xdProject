#include "mainwindow.h"
#include <QApplication>
#include "xdlib/readslice.h"
#include <QtDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
//    xd::TriangleMesh bb;
//    bb.ReadSTLFile("C:/Users/Administrator/Desktop/car.stl");
//    qDebug()<<"facet Number is:" <<bb.facets_count()<<'\n'
//            <<"needed repair?"<<bb.needed_repair();
    return a.exec();
}
