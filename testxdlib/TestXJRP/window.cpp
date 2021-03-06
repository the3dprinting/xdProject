#include "draw.h"
#include "window.h"

#include <QtWidgets>
#include <QFileDialog>

Window::Window()
{
    drawArea = new draw;
    model = new XJRP::SLCModel;
    triangleMesh = new xd::TriangleMesh;
    //用来显示层的SpinBox
    LayerNum = new QSpinBox;
    LayerNum->setRange(0,0);
    LayerNum->setSpecialValueText(tr("0 (No Model)"));
    connect(LayerNum, SIGNAL(valueChanged(int)),
            this, SLOT(LayerChanged()));
    LayerNumLabel = new QLabel(tr("Layer Number:"));
    LayerNumLabel->setBuddy(LayerNum);
    //用来设置填充模式的combobox
    infillPatternLabel = new QLabel(tr("Infill Pattern:"));
    infillPatternComboBox = new QComboBox;
    infillPatternComboBox->addItem(tr("line"));
    infillPatternComboBox->addItem(tr("skin_core"));
    connect(infillPatternComboBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(infillPatternChanged(int)));
    //用来设置参数的LineEdit
    interval_numEdit = new QLineEdit(tr("2"));
    interval_numLabel = new QLabel(tr("Interval Number:"));
    interval_numLabel->setBuddy(interval_numEdit);
    spaceEdit = new QLineEdit(tr("0.1"));
    spaceLabel = new QLabel(tr("Space:"));
    spaceLabel->setBuddy(spaceEdit);
    shrinkDistanceEdit = new QLineEdit(tr("0.1"));
    shrinkDistanceLabel = new QLabel(tr("Shrink Distance:"));
    shrinkDistanceLabel->setBuddy(shrinkDistanceEdit);
    angle_startEdit = new QLineEdit(tr("0"));
    angle_startLabel = new QLabel(tr("Angle Start:"));
    angle_startLabel->setBuddy(angle_startEdit);
    angle_deltaEdit = new QLineEdit(tr("90"));
    angle_deltaLabel = new QLabel(tr("angle Delta:"));
    angle_deltaLabel->setBuddy(angle_deltaEdit);
    LaserPowerEdit = new QLineEdit(tr("0.079"));
    LaserPowerLabel = new QLabel(tr("Laser Power:"));
    LaserPowerLabel->setBuddy(LaserPowerEdit);
    ScanSpeedEdit = new QLineEdit(tr("3000"));
    ScanSpeedLabel = new QLabel(tr("Scan Speed:"));
    ScanSpeedLabel->setBuddy(ScanSpeedEdit);
    contourNumLabel = new QLabel(tr("Contour Number:"));
    contourNumEdit = new QLineEdit(tr("1"));
    contourSpaceLabel = new QLabel("Contour Space:");
    contourSpaceEdit = new QLineEdit(tr("0.1"));
    thickness = new QLabel("thickness:");
    thicknessEdit = new QLineEdit(tr("0.1"));
    //按钮
    open = new QPushButton(tr("&open"));
    save = new QPushButton(tr("&save"));
    clear = new QPushButton(tr("&clear"));
    infill = new QPushButton(tr("&infill"));
    centering = new QPushButton(tr("center"));
    openSTL = new QPushButton(tr("openSTL"));
    slice = new QPushButton(tr("slice"));
    connect(open,SIGNAL(clicked()),
            this,SLOT(openTrigger()));
    connect(save,SIGNAL(clicked()),
            this,SLOT(saveTrigger()));
    connect(clear,SIGNAL(clicked()),
            this,SLOT(clearTrigger()));
    connect(infill,SIGNAL(clicked()),
            this,SLOT(infillTrigger()));
    connect(centering,SIGNAL(clicked()),
            drawArea,SLOT(centering()));
    connect(openSTL,SIGNAL(clicked()),
            this,SLOT(openSTLTrigger()));
    connect(slice,SIGNAL(clicked()),
    this,SLOT(sliceTrigger()));
    //布局
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(drawArea, 0, 0, 1, 16);
    mainLayout->addWidget(open,2, 0);
    mainLayout->addWidget(clear,2,1);
    mainLayout->addWidget(LayerNumLabel, 2, 2);
    mainLayout->addWidget(LayerNum, 2, 3);
    mainLayout->addWidget(centering,2,4);
    mainLayout->addWidget(infillPatternLabel,2,5);
    mainLayout->addWidget(infillPatternComboBox,2,6);
    mainLayout->addWidget(infill, 2, 7);
    mainLayout->addWidget(save, 2, 8);
    mainLayout->addWidget(openSTL,2,11);
    mainLayout->addWidget(thicknessEdit,2,12);
    mainLayout->addWidget(thickness,2,13);
    mainLayout->addWidget(slice,2,14);
    //第3行
    mainLayout->addWidget(contourNumLabel,3,0);
    mainLayout->addWidget(contourNumEdit,3,1);
    mainLayout->addWidget(contourSpaceLabel,3,2);
    mainLayout->addWidget(contourSpaceEdit,3,3);
    mainLayout->addWidget(spaceLabel,3,4);
    mainLayout->addWidget(spaceEdit,3,5);
    mainLayout->addWidget(shrinkDistanceLabel,3,6);
    mainLayout->addWidget(shrinkDistanceEdit,3,7);
    mainLayout->addWidget(angle_startLabel,3,8);
    mainLayout->addWidget(angle_startEdit,3,9);
    mainLayout->addWidget(angle_deltaLabel,3,10);
    mainLayout->addWidget(angle_deltaEdit,3,11);
    mainLayout->addWidget(LaserPowerLabel,3,12);
    mainLayout->addWidget(LaserPowerEdit,3,13);
    mainLayout->addWidget(ScanSpeedLabel,3,14);
    mainLayout->addWidget(ScanSpeedEdit,3,15);
    //试试放在第三行！
    mainLayout->addWidget(interval_numLabel,3,0);
    mainLayout->addWidget(interval_numEdit,3,1);
    setLayout(mainLayout);
    //LayerChanged();
    setWindowTitle(tr("SLC Read And Infill"));
    infill->hide();
    save->hide();
    centering->hide();
    interval_numEdit->hide();
    interval_numLabel->hide();
}

Window::~Window()
{
    if(this->triangleMesh)
    {
        delete this->triangleMesh;
        this->triangleMesh = NULL;
    }
}



void Window::LayerChanged()
{
    drawArea->setLayer(model->layerAtIndex(LayerNum->value()-1));
}

void Window::openTrigger()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open a slc file"),tr("/select .slc file please"),tr("slicing file(*.slc)"));
    if(fileName.length()==0)
        return;
    if(model->empty())
        this->model->readFile(fileName);
    else
    {
        XJRP::SLCModel tem;
        tem.readFile(fileName);
        this->model->merge(tem);
    }
    drawArea->setModel(*(this->model));
    LayerNum->setRange(1,this->model->size());
    LayerNum->setSpecialValueText(tr("1 (Layer Number)"));
    drawArea->setLayer(model->layerAtIndex(LayerNum->value()-1));
    infill->show();
    centering->show();
}

void Window::openSTLTrigger()
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
}

void Window::sliceTrigger()
{
    if(this->triangleMesh->stl.stats.original_num_facets==0)
    {
        QMessageBox::information(NULL, "Warning", "Not Loaded stl file", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    xd::TriangleMeshSlicer slicer(this->triangleMesh);
    std::vector<float> z;
    float sliceThinkness = this->thicknessEdit->text().toFloat();
    float top = this->triangleMesh->stl.stats.max.z;
    float bottom = this->triangleMesh->stl.stats.min.z;
    for(float deltaZ = bottom + sliceThinkness ; deltaZ < top ; deltaZ+=sliceThinkness)
        z.push_back(deltaZ);
    qDebug()<<"facet Number is:" <<slicer.mesh->facets_count()<<'\n'
            <<"needed repair："<<slicer.mesh->needed_repair();
    std::vector<xd::ExPolygons>* layers = new std::vector<xd::ExPolygons>;  //一定要new一下，这样下面才能用！
    slicer.slice(z,layers);
    QMessageBox::information(NULL, "remind", "slice finished", QMessageBox::Yes, QMessageBox::Yes);
    this->model->readxdlib(z,layers);
    delete layers;
    drawArea->setModel(*(this->model));
    LayerNum->setRange(1,this->model->size());
    LayerNum->setSpecialValueText(tr("1 (Layer Number)"));
    drawArea->setLayer(model->layerAtIndex(LayerNum->value()-1));
    infill->show();
    centering->show();

    openSTL->hide();
    thickness->hide();
    thicknessEdit->hide();
    slice->hide();
}

void Window::saveTrigger()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save a xlc file"),"/home/jana/untitled.xlc",tr("slicing file(*.xlc)"));
    if(fileName.length()==0)
        return;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return;
    QDataStream temS(&file);
    temS<<*(QList<XJRP::Layer> *)(this->model);
    file.close();
}
void Window::clearTrigger()
{
    this->model->clear();
    if(this->triangleMesh)
        delete this->triangleMesh;
    this->triangleMesh = new xd::TriangleMesh;
    LayerNum->setRange(0,0);
    LayerNum->setSpecialValueText(tr("0 (No Model)"));
    infill->hide();
    save->hide();
    centering->hide();

    openSTL->show();
    thickness->show();
    thicknessEdit->show();
    slice->show();
}
void Window::infillTrigger()
{
      for(int i=0;i!=this->model->count() ;++i)
       {
           this->model->operator [](i).unfill();
       }
//    for(int i=0 ; i < this->model->size();++i)
//    {
//        for(int j=0; j < this->model->operator[](i).size();++j)
//        {
//            if(this->model->operator [](i)[j].type()==XJRP::Polygon::PolygonType::Infill)
//            {
//                this->model->operator [](i).erase(this->model->operator [](i).begin()+j);
//                --j;
//            }
//        }
//    }
    switch (infillPatternComboBox->currentIndex()) {
    case 1:
         this->model->skin_core_infill(interval_numEdit->text().toInt(),
                                  spaceEdit->text().toFloat(),
                                  shrinkDistanceEdit->text().toFloat(),
                                  angle_startEdit->text().toFloat(),
                                  angle_deltaEdit->text().toFloat(),
                                  LaserPowerEdit->text().toDouble(),
                                  ScanSpeedEdit->text().toDouble());
        break;
     case 0:
        this->model->line_infill(contourNumEdit->text().toInt(),
                                  contourSpaceEdit->text().toFloat(),
                                  spaceEdit->text().toFloat(),
                                  shrinkDistanceEdit->text().toFloat(),
                                  angle_startEdit->text().toFloat(),
                                  angle_deltaEdit->text().toFloat(),
                                  LaserPowerEdit->text().toDouble(),
                                  ScanSpeedEdit->text().toDouble());
        break;
    default:
        break;
    }
    drawArea->setLayer(model->layerAtIndex(LayerNum->value()-1));
    save->show();
}

void Window::infillPatternChanged(int N)
{
    switch (N)
    {
        case 0:
            interval_numEdit->hide();
            interval_numLabel->hide();
            contourNumEdit->show();
            contourNumLabel->show();
            contourSpaceEdit->show();
            contourSpaceLabel->show();
            break;
        case 1:
            interval_numEdit->show();
            interval_numLabel->show();
            contourNumEdit->hide();
            contourNumLabel->hide();
            contourSpaceEdit->hide();
            contourSpaceLabel->hide();
            break;
        default:
            break;
    }
}
