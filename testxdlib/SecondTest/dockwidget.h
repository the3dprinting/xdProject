#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QDockWidget>


QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QLineEdit;
QT_END_NAMESPACE

class dockwidget : public QDockWidget
{
    Q_OBJECT

public:

    dockwidget(QWidget *parent = 0);

    QPushButton *SliceThicknessButton;   //��Ҫ���У�Ϊ����������ʹ��
    QPushButton *centerButton;           //��Ҫ���У�Ϊ����������ʹ��
    QPushButton *medialAxisButton;
    QPushButton *trapezoidsButton;
    QPushButton *trapezoids2Button;
    QPushButton *triangulateButton;
    QPushButton *triangulate_ppButton;
    QPushButton *triangulate_p2tButton;
    QLineEdit *SliceThicknessEdit;       //��Ҫ���У�Ϊ����������ʹ��
    QSpinBox *LayerNum;                  //��Ҫ���У�Ϊ����������ʹ��

public slots:

    void setLayerRange(int lr);

signals:

    //void sliceButtonClicked();

protected:
//    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
//    void wheelEvent(QWheelEvent *e);
//    void mousePressEvent(QMouseEvent *e);
//    void mouseMoveEvent(QMouseEvent *e);

private:
    //QWidget* dockWidgetContents;
    QLabel *SliceThicknessLabel;
    QLabel *LayerNumLabel;

};

#endif // DOCKWIDGET_H

