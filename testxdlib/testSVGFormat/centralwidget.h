#ifndef CENTRALWIDGET
#define CENTRALWIDGET

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include "xdlib/exPolygon.h"

class  MainWindow;

class centralwidget : public QWidget
{
    Q_OBJECT

public:

    centralwidget(QWidget *parent = 0);
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;   //定义本身widget的最小尺寸
    QSize sizeHint() const Q_DECL_OVERRIDE;   //返回本身widget的推荐值
    void paint(QPainter &painter);

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
    xd::ExPolygons * polygonsToDraw;    //需要画的多边形的数据
    std::vector<xd::Polylines> * medialAxisToDraw;    //需要画的中轴线的数据
    std::vector<xd::Polygons> * trToDraw;   //

    QPointF lastPos;
    qreal moveX;
    qreal moveY;
    qreal scale; //放缩系数
    QPen pen;
    QBrush brush;

friend MainWindow;  //友元，为了让主窗口访问
};

#endif // CENTRALWIDGET

