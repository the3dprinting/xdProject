﻿#ifndef XJSLCMODEL_H
#define XJSLCMODEL_H

#include <QList>
#include "xjlayer.h"
#include "xdlib/exPolygon.h"
#include "xdlib/constdefine.h"

namespace XJRP
{

class SLCModel :public QList<Layer>
{
public:
    const Boundary boundary () const;
    const xjPoint center () const;
    const xjPoint dimension () const;
    const QList <qreal> heights () const;
    const Layer layerAtIndex (const int index) const;
    const Layer layerAtHeight (const qreal height) const;
    void sort ();
    void merge(const SLCModel &other);
    bool readFile (const QString &filename);
    void readxdlib (std::vector<float> z,std::vector<xd::ExPolygons>* layers);  //内部需要改动轮廓方向，不能使用const关键字
    void skin_core_infill(int interval_num,float space,float shrinkDistance,float angle_start,float angle_delta,qreal LaserPower=-1.0,qreal ScanSpeed=-1.0);   //测试用函数
    void line_infill(qint8 ContourCount, qreal ContourWidth,float space,float shrinkDistance,float angle_start,float angle_delta,qreal LaserPower=-1.0,qreal ScanSpeed=-1.0);   //测试用函数


private:
    QList<qreal> layerHeights;
};

}

#endif // SLCMODEL_H
