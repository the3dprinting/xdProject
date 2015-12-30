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
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;   //���屾��widget����С�ߴ�
    QSize sizeHint() const Q_DECL_OVERRIDE;   //���ر���widget���Ƽ�ֵ


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
    xd::ExPolygons * polygonsToDraw;
    std::vector<xd::Polylines> * medialAxisToDraw;
    std::vector<xd::Polygons> * trToDraw;

    QPointF lastPos;
    qreal moveX;
    qreal moveY;
    qreal scale; //����ϵ��
    QPen pen;
    QBrush brush;

friend MainWindow;  //��Ԫ��Ϊ���������ڷ���
};

#endif // CENTRALWIDGET

