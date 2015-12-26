#ifndef CENTRALWIDGET
#define CENTRALWIDGET

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include "xdlib/exPolygon.h"

class centralwidget : public QWidget
{
    Q_OBJECT

public:

    centralwidget(QWidget *parent = 0);
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;   //定义本身widget的最小尺寸
    QSize sizeHint() const Q_DECL_OVERRIDE;   //返回本身widget的推荐值
    xd::ExPolygons * polygonsToDraw;   //共有为了让主窗口访问
    qreal scale; //放缩系数暂时放这里

    std::vector<xd::Polylines> * medialAxisToDraw;
    std::vector<xd::Polygons> * trToDraw;

public slots:

    void centering();
    void generate_media_axis();
    void generate_trapezoids(double angle);
    void generate_trapezoids2(double angle);
    void generate_triangulate();
    void generate_triangulate_pp();
    void generate_triangulate_p2t();
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:

    QPointF lastPos;
    qreal moveX;
    qreal moveY;
    QPen pen;
    QBrush brush;
};

#endif // CENTRALWIDGET

