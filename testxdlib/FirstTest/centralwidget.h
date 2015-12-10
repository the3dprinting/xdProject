#ifndef CENTRALWIDGET
#define CENTRALWIDGET

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>

class centralwidget : public QWidget
{
    Q_OBJECT

public:

    centralwidget(QWidget *parent = 0);
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;   //���屾��widget����С�ߴ�
    QSize sizeHint() const Q_DECL_OVERRIDE;   //���ر���widget���Ƽ�ֵ
    //void setModel(const XJRP::SLCModel & M);
    //void setLayer(const XJRP::Layer & L);
public slots:

    void centering();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:
    QPointF lastPos;
    qreal moveX;
    qreal moveY;
    qreal scale;
    QPen pen;
    QBrush brush;
};

#endif // CENTRALWIDGET

