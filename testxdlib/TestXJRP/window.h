#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "xjrp/xjslcmodel.h"
#include "xdlib/readSlice.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QSpinBox;
class QPushButton;
class QLineEdit;
QT_END_NAMESPACE
class draw;

//! [0]
class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window();
    ~Window();
private slots:
    void LayerChanged();
    void openTrigger();
    void openSTLTrigger();
    void saveTrigger();
    void sliceTrigger();
    void clearTrigger();
    void infillTrigger();
    void infillPatternChanged(int N);
private:
    XJRP::SLCModel *model;
    xd::TriangleMesh *triangleMesh;
    //std::vector<xd::ExPolygons>* layers;

    draw *drawArea;
    QLabel *LayerNumLabel;
    QSpinBox *LayerNum;
    //for skin_core
    QLabel *interval_numLabel;
    QLineEdit *interval_numEdit;
    QLabel *spaceLabel;
    QLineEdit *spaceEdit;
    //common
    QLabel *contourNumLabel;
    QLineEdit *contourNumEdit;
    QLabel *contourSpaceLabel;
    QLineEdit *contourSpaceEdit;
    QLabel *shrinkDistanceLabel;
    QLineEdit *shrinkDistanceEdit;
    QLabel *angle_startLabel;
    QLineEdit *angle_startEdit;
    QLabel *angle_deltaLabel;
    QLineEdit *angle_deltaEdit;
    QLabel *LaserPowerLabel;
    QLineEdit *LaserPowerEdit;
    QLabel *ScanSpeedLabel;
    QLineEdit *ScanSpeedEdit;

    QPushButton *open;
    QPushButton *save;
    QPushButton *clear;
    QPushButton *infill;
    QPushButton *centering;
    //slt文件功能
    QPushButton *openSTL;
    QPushButton *slice;
    QLabel *thickness;
    QLineEdit *thicknessEdit;

    QLabel *infillPatternLabel;
    QComboBox *infillPatternComboBox;
};
//! [0]

#endif // WINDOW_H

