#include "centralwidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QtMath>
#include "xdlib/multiPoint.h"
#include "xdlib/exPolygon.h"
#include "xdlib/line.h"
#include "xdlib/constdefine.h"


void _drawArrow(QPainter *p, const QPointF & A, const QPointF & B)  //根据传入线段的端点画箭头
{
    QPointF temB(std::sqrt(std::pow((A.x()-B.x()), 2) + std::pow(A.y()-B.y(), 2)),0);
    p->save();
    p->setPen (QPen (Qt::darkYellow));
    //想不通，要先平移旋转！
    p->translate(A);
    p->rotate(qRadiansToDegrees(std::atan2(B.y()-A.y(),B.x()-A.x())));

    p->drawLine(temB.x(),temB.y(),temB.x()-5,temB.y()+4);
    p->drawLine(temB.x(),temB.y(),temB.x()-5,temB.y()-4);
    p->restore();
}

void _drawAxis(QPainter *p)   //画坐标轴
{
    QPointF xNegative(-100,0);
    QPointF xPositive(100,0);
    QPointF yNegative(0,-100);
    QPointF yPositive(0,100);
    p->setPen(Qt::darkMagenta);
    p->drawLine(xNegative,xPositive);
    p->drawLine(yNegative,yPositive);
    _drawArrow(p,xNegative,xPositive);
    _drawArrow(p,yNegative,yPositive);
    QPen pen(Qt::darkRed);
    pen.setWidth(5);
    p->setPen(pen);
    p->drawPoint(QPointF(0,0));
}

void _drawPolygon(QPainter *p, const QPolygonF &polygon)  //画QPolygonF
{
    //temp
    p->drawPolygon(polygon);
    QPointF A=polygon.operator [](polygon.size()-2);
    QPointF B=polygon.operator [](polygon.size()-1);
    _drawArrow(p,A,B);
    //下面是强调起点的标志
    QPen pen(Qt::darkYellow);
    pen.setWidth(4);
    p->setPen(pen);
    p->drawPoint(polygon.front());
}

void _drawMultiPoint(QPainter *p, const xd::MultiPoint & MP,qreal scale)   //画Layer
{
    xd::Lines lines=MP.lines();
    for(int i=0;i!=lines.size();++i)
    {
        p->drawLine(scale*QPointF(lines[i].a.x*SCALING_FACTOR,scale*lines[i].a.y*SCALING_FACTOR),
                    scale*QPointF(lines[i].b.x*SCALING_FACTOR,scale*lines[i].b.y*SCALING_FACTOR));
    }
}

void _drawExPolygon(QPainter *p, const xd::ExPolygon & EP,qreal scale)
{
    p->setPen(Qt::red);
    _drawMultiPoint(p,EP.contour,scale);
    p->setPen(Qt::green);
    for(int i=0; i!=EP.holes.size();++i)
    {
        _drawMultiPoint(p,EP.holes[i],scale);
    }
}

void _drawPolylines(QPainter *p, const xd::Polylines & PL,qreal scale)
{
    p->setPen(Qt::blue);
    for(xd::Polylines::const_iterator i = PL.begin() ; i!=PL.end();++i)
    {
        _drawMultiPoint(p,*i,scale);
    }
}

void _drawPolygons(QPainter *p, const xd::Polygons & PG,qreal scale)
{
    p->setPen(Qt::black);
    for(xd::Polygons::const_iterator i = PG.begin() ; i!=PG.end();++i)
    {
        _drawMultiPoint(p,*i,scale);
    }
}

centralwidget::centralwidget(QWidget *parent)
    : QWidget(parent)
{
    this->polygonsToDraw = new xd::ExPolygons;
    this->medialAxisToDraw = new std::vector<xd::Polylines>;
    this->trToDraw = new std::vector<xd::Polygons>;
//    moveX=this->width()/2*devicePixelRatio();
//    moveY=this->height()/2*devicePixelRatio();
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    this->scale = 1; //开始系数为1
}

QSize centralwidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize centralwidget::sizeHint() const
{
    return QSize(800, 400);
}

void centralwidget::centering()
{
    moveX=this->width()/2*devicePixelRatio();
    moveY=this->height()/2*devicePixelRatio();
    update();
}

void centralwidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter;
    /*
    int side = qMin(width(), height());
    int x = (width() - side / 2);
    int y = (height() - side / 2);
    painter.setViewport(x, y, side, side);  //将屏幕坐标系和物理坐标系对应
    */
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(painter);
    painter.end();
}

//! [paint function]
void centralwidget::paint(QPainter &painter)
{
//![paint picture]
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.translate(moveX,moveY);
    //painter.rotate(-90);  //坐标屏幕翻转
    //_drawAxis(&painter);
    for(xd::ExPolygons::const_iterator i=this->polygonsToDraw->begin() ; i!=this->polygonsToDraw->end() ; ++i)
    {
        _drawExPolygon(&painter,*i,this->scale);
    }
    for(std::vector<xd::Polylines>::const_iterator i=this->medialAxisToDraw->begin() ; i!=this->medialAxisToDraw->end() ; ++i)
    {
        _drawPolylines(&painter,*i,this->scale);
    }
    for(std::vector<xd::Polygons>::const_iterator i=this->trToDraw->begin() ; i!=this->trToDraw->end() ; ++i)
    {
        _drawPolygons(&painter,*i,this->scale);
    }
//![paint picture]
}
//! [paint function]

void centralwidget::wheelEvent(QWheelEvent *e)
{
    double numDegrees = -e->delta() / 8.0;
    double numSteps = numDegrees / 15.0;
    scale /= pow(1.125, numSteps);
    update();
}

void centralwidget::mousePressEvent(QMouseEvent *e)
{
    lastPos = e->pos();
}

void centralwidget::mouseMoveEvent(QMouseEvent *e)
{
    qreal dx = qreal(e->x() - lastPos.x())/devicePixelRatio() ;
    qreal dy = qreal(e->y() - lastPos.y())/devicePixelRatio() ;
    if (e->buttons() & Qt::LeftButton) {
        moveX+=dx;
        moveY+=dy;
        update();
    } else if (e->buttons() & Qt::RightButton) {

        moveX+=dx*10;
        moveY+=dy*10;
        update();
    }
    else if(e->buttons() & Qt::MidButton)
    {

        update();
    }
    lastPos = e->pos();
}

//槽函数的实现
void centralwidget::generate_media_axis()
{
    if(this->polygonsToDraw->empty())
        return;
    this->medialAxisToDraw->clear();
    this->trToDraw->clear();
    for(xd::ExPolygons::const_iterator i=this->polygonsToDraw->begin() ; i!=this->polygonsToDraw->end() ; ++i)
    {
        xd::Polylines tp;
        i->medial_axis(1,0.1,&tp);
        this->medialAxisToDraw->push_back(tp);
    }
    update();
}

void centralwidget::generate_trapezoids(double angle)
{
    if(this->polygonsToDraw->empty())
        return;
    this->trToDraw->clear();
    this->medialAxisToDraw->clear();
    for(xd::ExPolygons::const_iterator i=this->polygonsToDraw->begin() ; i!=this->polygonsToDraw->end() ; ++i)
    {
        xd::Polygons tp;
        i->get_trapezoids(&tp,angle);
        this->trToDraw->push_back(tp);
    }
    update();
}
void centralwidget:: generate_trapezoids2(double angle)
{
    if(this->polygonsToDraw->empty())
        return;
    this->trToDraw->clear();
    this->medialAxisToDraw->clear();
    for(xd::ExPolygons::const_iterator i=this->polygonsToDraw->begin() ; i!=this->polygonsToDraw->end() ; ++i)
    {
        xd::Polygons tp;
        i->get_trapezoids2(&tp,angle);
        this->trToDraw->push_back(tp);
    }
    update();
}
void centralwidget::generate_triangulate()
{
//    if(this->polygonsToDraw->empty())
//        return;
    this->trToDraw->clear();
    this->medialAxisToDraw->clear();
    for(xd::ExPolygons::const_iterator i=this->polygonsToDraw->begin() ; i!=this->polygonsToDraw->end() ; ++i)
    {
        xd::Polygons  tp;   //太奇怪了，这里不能用  *tp  ！！！
        i->triangulate(&tp);
        this->trToDraw->push_back(tp);
    }
    update();
}
void centralwidget::generate_triangulate_pp()
{
    if(this->polygonsToDraw->empty())
        return;
    this->trToDraw->clear();
    this->medialAxisToDraw->clear();
    for(xd::ExPolygons::const_iterator i=this->polygonsToDraw->begin() ; i!=this->polygonsToDraw->end() ; ++i)
    {
        xd::Polygons  tp;
        i->triangulate_pp(&tp);
        this->trToDraw->push_back(tp);
    }
    update();
}
void centralwidget::generate_triangulate_p2t()
{
    if(this->polygonsToDraw->empty())
        return;
    this->trToDraw->clear();
    this->medialAxisToDraw->clear();
    for(xd::ExPolygons::const_iterator i=this->polygonsToDraw->begin() ; i!=this->polygonsToDraw->end() ; ++i)
    {
        xd::Polygons  tp;
        i->triangulate_p2t(&tp);
        this->trToDraw->push_back(tp);
    }
    update();
}
