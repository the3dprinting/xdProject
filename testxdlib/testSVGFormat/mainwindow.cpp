#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtDebug>
#include <QMessageBox>
#include <QLineEdit>
#include <QSvgGenerator>
#include <QPainter>
#include "xdlib/expolygon.h"
#include "xdlib/polygon.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(720,540);
    this->cw = new centralwidget(this);
    this->dw = new dockwidget(this);
    this->triangleMesh = new xd::TriangleMesh;   //这里必须先要声明，否则后面函数不能使用它！
    this->layers = new std::vector<xd::ExPolygons>;
    setCentralWidget(cw);
    addDockWidget(Qt::LeftDockWidgetArea,dw);
    connect((QObject*)dw->SliceThicknessButton,SIGNAL(clicked()),this,SLOT(on_sliceButton_clicked()));  //这个需要强制转换
    connect((QObject*)dw->centerButton,SIGNAL(clicked()),this,SLOT(on_centerButton_clicked()));
    connect((QObject*)dw->LayerNum,SIGNAL(valueChanged(int)),this,SLOT(layerNumChanged(int)));
//    connect((QObject*)dw->triangulateButton,SIGNAL(clicked()),this->cw,SLOT(generate_triangulate()));
//    connect((QObject*)dw->triangulate_ppButton,SIGNAL(clicked()),this->cw,SLOT(generate_triangulate_pp()));
//    connect((QObject*)dw->triangulate_p2tButton,SIGNAL(clicked()),this->cw,SLOT(generate_triangulate_p2t()));
//    connect((QObject*)dw->medialAxisButton,SIGNAL(clicked()),this->cw,SLOT(generate_media_axis()));
    connect(this,SIGNAL(changeLayerNumRange(int)),dw,SLOT(setLayerRange(int)));
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

void MainWindow::on_centerButton_clicked()
{
    this->cw->centering();
}

void MainWindow::layerNumChanged(int i)
{
    //计算这一层的多边形的总外包围盒
    xd::ExPolygons * ep = & this->layers->operator [](i-1) ;
    xd::BoundingBox bb;
    for(std::vector<xd::ExPolygon>::const_iterator i = ep->begin() ; i != ep->end() ; ++i)
    {
        xd::Polygons temP = *i;
        for(std::vector<xd::Polygon>::const_iterator j = temP.begin() ; j != temP.end() ; ++j)
            bb.merge(j->bounding_box());
    }
    size = bb.size();
    cw->moveX = 0;
    cw->moveY = 0;
    cw->moveX -= bb.min.x * SCALING_FACTOR;
    cw->moveY -= bb.min.y * SCALING_FACTOR;

    //this->cw->polygonsToDraw->clear();
    this->cw->trToDraw->clear();
    this->cw->medialAxisToDraw->clear();
    this->cw->polygonsToDraw = & this->layers->operator [](i-1) ;
    this->cw->update();
}


void MainWindow::on_action_S_triggered()   //点击保存时触发的槽函数
{
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save SVG"),
        path, tr("SVG files (*.svg)"));

    if (newPath.isEmpty())
        return;

    path = newPath;

//![configure SVG generator]
    QSvgGenerator generator;
    generator.setFileName(path);
    generator.setSize(QSize(size.x*SCALING_FACTOR*10, size.y*SCALING_FACTOR*10));     //在生成的SVG文件的对应标签中显示该文件的大小
    generator.setViewBox(QRect(0, 0, size.x *SCALING_FACTOR , size.y *SCALING_FACTOR));
    generator.setTitle(tr("SVG Generator Example Drawing"));
    generator.setDescription(tr("An SVG drawing created by the SVG Generator "
                                "Example provided with Qt."));
//![configure SVG generator]
//![begin painting]
    QPainter painter;
    painter.begin(&generator);
//![begin painting]
    cw->paint(painter);
//![end painting]
    painter.end();
//![end painting]
}
