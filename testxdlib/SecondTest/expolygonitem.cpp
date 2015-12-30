#include "expolygonitem.h"
#include "xdlib/boundingBox.h"
#include <QPainter>

void _drawMultiPoint(QPainter *p, const xd::MultiPoint & MP)   //画Layer
{
    xd::Lines lines=MP.lines();
    for(int i=0;i!=lines.size();++i)
    {
        p->drawLine(QPointF(lines[i].a.x*SCALING_FACTOR,lines[i].a.y*SCALING_FACTOR),
                    QPointF(lines[i].b.x*SCALING_FACTOR,lines[i].b.y*SCALING_FACTOR));
    }
}

void _drawExPolygon(QPainter *p, const xd::ExPolygon & EP)
{
    p->setPen(Qt::red);
    _drawMultiPoint(p,EP.contour);
    p->setPen(Qt::green);
    for(int i=0; i!=EP.holes.size();++i)
    {
        _drawMultiPoint(p,EP.holes[i]);
    }
}

void _drawPolylines(QPainter *p, const xd::Polylines & PL)
{
    p->setPen(Qt::blue);
    for(xd::Polylines::const_iterator i = PL.begin() ; i!=PL.end();++i)
    {
        _drawMultiPoint(p,*i);
    }
}

void _drawPolygons(QPainter *p, const xd::Polygons & PG)
{
    p->setPen(Qt::black);
    for(xd::Polygons::const_iterator i = PG.begin() ; i!=PG.end();++i)
    {
        _drawMultiPoint(p,*i);
    }
}

//开始类的定义************************************************//
expolygonitem::expolygonitem(xd::ExPolygons * e)
{
    exPolygonToDraw = e;
    medialAxisToDraw = new std::vector<xd::Polylines>;
    trToDraw= new std::vector<xd::Polygons>;
}

expolygonitem::expolygonitem(std::vector<xd::Polylines> *e)
{
    exPolygonToDraw = new xd::ExPolygons;
    medialAxisToDraw = e;
    trToDraw= new std::vector<xd::Polygons>;
}

expolygonitem::expolygonitem(std::vector<xd::Polygons> *e)
{
    exPolygonToDraw = new xd::ExPolygons;
    medialAxisToDraw = new std::vector<xd::Polylines>;
    trToDraw= e;
}

QRectF expolygonitem::boundingRect() const
{
    if (exPolygonToDraw->empty())
        return QRectF();
    qreal penWidth = 1;
    qreal extra = penWidth / 2.0;

    xd::Points allPoints;
    for(xd::ExPolygons::const_iterator i=exPolygonToDraw->begin();i!=exPolygonToDraw->end();++i)
    {
        xd::Points points = i->points();
        allPoints.insert(allPoints.end(),points.begin(),points.end());
    }
    xd::BoundingBox box(allPoints);
    QPointF topLeft(box.min.x * SCALING_FACTOR, box.max.y * SCALING_FACTOR);
    QPointF bottomRight (box.max.x * SCALING_FACTOR,box.min.y * SCALING_FACTOR);
    return QRectF(topLeft, bottomRight).normalized().adjusted(-extra, -extra, extra, extra);
}

void expolygonitem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
     for(xd::ExPolygons::const_iterator i=this->exPolygonToDraw->begin() ; i!=this->exPolygonToDraw->end() ; ++i)
    {
        _drawExPolygon(painter,*i);
    }
    for(std::vector<xd::Polylines>::const_iterator i=this->medialAxisToDraw->begin() ; i!=this->medialAxisToDraw->end() ; ++i)
    {
        _drawPolylines(painter,*i);
    }
    for(std::vector<xd::Polygons>::const_iterator i=this->trToDraw->begin() ; i!=this->trToDraw->end() ; ++i)
    {
        _drawPolygons(painter,*i);
    }
}


