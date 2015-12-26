#include "dockwidget.h"
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>

dockwidget::dockwidget(QWidget *parent)
    : QDockWidget(parent)
{

    setFeatures(QDockWidget::DockWidgetVerticalTitleBar|QDockWidget::DockWidgetMovable);  //可移动、可有下滑栏
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea); //允许移动的区域
    //用来切片的LineEdit
    SliceThicknessEdit = new QLineEdit(tr("0.2"));
    SliceThicknessLabel = new QLabel(tr("Slice Thickness:"));
    SliceThicknessLabel->setBuddy(SliceThicknessEdit);
    SliceThicknessButton = new QPushButton(tr("slice"));
    //用来显示层的SpinBox
    LayerNum = new QSpinBox;
    LayerNum->setRange(0,0);
    LayerNum->setSpecialValueText(tr("0 (No Model)"));  //变化的信号和槽的connect在父类实现
    LayerNumLabel = new QLabel(tr("Layer Number:"));
    LayerNumLabel->setBuddy(LayerNum);
    //用来显示到中心的按钮
    centerButton = new QPushButton(tr("center"));
    //下面为测试用按钮
    triangulateButton = new QPushButton(tr("triangulate"));
    triangulate_ppButton = new QPushButton(tr("triangulate_pp"));
    triangulate_p2tButton = new QPushButton(tr("triangulate_p2t"));
    medialAxisButton = new QPushButton(tr("medialAxis"));
    //下面布局
    QWidget * dockWidgetContents = new QWidget(this);   //dockwidget必须设置多个项目在一个widget上，不能直接加上layout
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(SliceThicknessLabel,0,0);
    mainLayout->addWidget(SliceThicknessEdit,0,1);
    mainLayout->addWidget(SliceThicknessButton,1,1);
    mainLayout->addWidget(centerButton,2,0);
    mainLayout->addWidget(LayerNumLabel,3,0);
    mainLayout->addWidget(LayerNum,3,1);
    mainLayout->addWidget(triangulateButton,4,0);
    mainLayout->addWidget(triangulate_ppButton,4,1);
    mainLayout->addWidget(triangulate_p2tButton,5,0);
    mainLayout->addWidget(medialAxisButton,5,1);
    dockWidgetContents->setLayout(mainLayout);
    setWidget(dockWidgetContents);
}

void dockwidget::setLayerRange(int lr)
{  
    LayerNum->setRange(1,lr);
    if(lr >= 1)
        LayerNum->setValue(1);
}


