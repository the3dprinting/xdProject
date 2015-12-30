#ifndef EXPOLYGONITEM
#define EXPOLYGONITEM

#include <QGraphicsItem>
#include "xdlib/exPolygon.h"

class expolygonitem : public QGraphicsItem
{
public:
    //enum drawType{exPolygon,mediaAxis,tri};
    expolygonitem(xd::ExPolygons * e);
    expolygonitem(std::vector<xd::Polylines> *e);
    expolygonitem(std::vector<xd::Polygons> *e);
    enum { Type = UserType + 1 };
    int type() const Q_DECL_OVERRIDE { return Type; }

protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

public slots:


private:
    xd::ExPolygons * exPolygonToDraw;
    std::vector<xd::Polylines> * medialAxisToDraw;
    std::vector<xd::Polygons> * trToDraw;
};

#endif // EXPOLYGONITEM

