#include "dockwidget.h"
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>

dockwidget::dockwidget(QWidget *parent)
    : QDockWidget(parent)
{

    setFeatures(QDockWidget::DockWidgetVerticalTitleBar|QDockWidget::DockWidgetMovable);  //���ƶ��������»���
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea); //�����ƶ�������
    //������Ƭ��LineEdit
    SliceThicknessEdit = new QLineEdit(tr("0.2"));
    SliceThicknessLabel = new QLabel(tr("Slice Thickness:"));
    SliceThicknessLabel->setBuddy(SliceThicknessEdit);
    SliceThicknessButton = new QPushButton(tr("slice"));
    //������ʾ���SpinBox
    LayerNum = new QSpinBox;
    LayerNum->setRange(0,0);
    LayerNum->setSpecialValueText(tr("0 (No Model)"));  //�仯���źźͲ۵�connect�ڸ���ʵ��
    LayerNumLabel = new QLabel(tr("Layer Number:"));
    LayerNumLabel->setBuddy(LayerNum);
    //������ʾ�����ĵİ�ť
    centerButton = new QPushButton(tr("center"));
    //����Ϊ�����ð�ť
    triangulateButton = new QPushButton(tr("triangulate"));
    triangulate_ppButton = new QPushButton(tr("triangulate_pp"));
    triangulate_p2tButton = new QPushButton(tr("triangulate_p2t"));
    medialAxisButton = new QPushButton(tr("medialAxis"));
    //���沼��
    QWidget * dockWidgetContents = new QWidget(this);   //dockwidget�������ö����Ŀ��һ��widget�ϣ�����ֱ�Ӽ���layout
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


