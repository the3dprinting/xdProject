#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtDebug>
#include <QMessageBox>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(720,540);
    //this->cw = new centralwidget(this);
    this->triangleMesh = new xd::TriangleMesh;   //这里必须先要声明，否则后面函数不能使用它！
    this->layers = new std::vector<xd::ExPolygons>;
    this->dw = new dockwidget(this);
    addDockWidget(Qt::LeftDockWidgetArea,dw);
    connect((QObject*)dw->SliceThicknessButton,SIGNAL(clicked()),this,SLOT(on_sliceButton_clicked()));  //这个需要强制转换
    connect(this,SIGNAL(changeLayerNumRange(int)),dw,SLOT(setLayerRange(int)));
    this->triangleMesh = new xd::TriangleMesh;   //这里必须先要声明，否则后面函数不能使用它！
    this->layers = new std::vector<xd::ExPolygons>;
}

MainWindow::~MainWindow()
{
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

void MainWindow::on_sliceButton_clicked()
{
    if(this->triangleMesh->stl.stats.original_num_facets==0)
    {
        QMessageBox::information(NULL, "Warning", "Not Loaded stl file", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    xd::TriangleMeshSlicer slicer(this->triangleMesh);
    std::vector<float> z;
    float sliceThinkness = dw->SliceThicknessEdit->text().toFloat();
    float top = this->triangleMesh->stl.stats.max.z;
    float bottom = this->triangleMesh->stl.stats.min.z;
    for(float deltaZ = bottom + sliceThinkness ; deltaZ < top ; deltaZ+=sliceThinkness)
        z.push_back(deltaZ);
    qDebug()<<"facet Number is:" <<slicer.mesh->facets_count()<<'\n'
            <<"needed repair："<<slicer.mesh->needed_repair();
    this->layers->clear();
    slicer.slice(z,this->layers);
    QMessageBox::information(NULL, "remind", "slice finished", QMessageBox::Yes, QMessageBox::Yes);
    changeLayerNumRange(z.size());
}
