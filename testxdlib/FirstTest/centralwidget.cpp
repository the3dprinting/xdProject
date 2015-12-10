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

centralwidget::centralwidget(QWidget *parent)
    : QWidget(parent)
{
//    moveX=this->width()/2*devicePixelRatio();
//    moveY=this->height()/2*devicePixelRatio();
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
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

//void centralwidget::setLayer(const Layer &L)
//{
//    this->layer = L;
//    update();
//}

//void centralwidget::setModel(const SLCModel &M)
//{
//    this->Model=M;
//    moveX=this->width()/2*devicePixelRatio();
//    moveY=this->height()/2*devicePixelRatio();
//    Boundary boundary (this->Model.boundary());
//    scale=(this->height()/(boundary.maxX () - boundary.minX ()));
//}

void centralwidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.translate(moveX,moveY);
    painter.rotate(-90);  //坐标屏幕翻转
    _drawAxis(&painter);
    //_drawLayer(&painter, layer, scale);

//    painter.setRenderHint(QPainter::Antialiasing, false);
//    painter.setPen(palette().dark().color());
//    painter.setBrush(Qt::NoBrush);
//    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

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
    qreal dx = qreal(e->x() - lastPos.x()) / scale;
    qreal dy = qreal(e->y() - lastPos.y()) / scale;
    if (e->buttons() & Qt::LeftButton) {
        moveX+=dx*15;
        moveY+=dy*15;
        update();
    } else if (e->buttons() & Qt::RightButton) {

        update();
    }
    else if(e->buttons() & Qt::MidButton)
    {

        update();
    }
    lastPos = e->pos();
}

