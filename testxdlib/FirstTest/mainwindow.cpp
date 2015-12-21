#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->cw = new centralwidget;
    this->triangleMesh = new xd::TriangleMesh;   //这里必须先要声明，否则后面函数不能使用它！
    setCentralWidget(cw);
}

MainWindow::~MainWindow()
{
    //delete triangleMesh;
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    std::string fileName = (QFileDialog::getOpenFileName(this,tr("Open a stl file"),tr("/select .stl file please"),tr("model file(*.stl)"))).toStdString();
    if(fileName.length()==0)
        return;
    char *FN;
    FN=new char[fileName.length()];
    memset(FN,'\0',(fileName.length())*sizeof(char));
    strcpy(FN,fileName.c_str());
    this->triangleMesh->ReadSTLFile(FN);
    qDebug()<<"facet Number is:" <<triangleMesh->facets_count()<<'\n'
            <<"needed repair："<<triangleMesh->needed_repair();
    //delete []FN;
}


