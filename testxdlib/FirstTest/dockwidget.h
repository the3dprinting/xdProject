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

class MainWindow;

class dockwidget : public QDockWidget
{
    Q_OBJECT

public:

    dockwidget(QWidget *parent = 0);
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
    QLineEdit *SliceThicknessEdit;       //主窗口使用
    QSpinBox *LayerNum;                  //主窗口使用

    QPushButton *SliceThicknessButton;   //主窗口使用
    QPushButton *centerButton;           //主窗口使用,下略...
    QPushButton *medialAxisButton;
    QPushButton *trapezoidsButton;
    QPushButton *trapezoids2Button;
    QPushButton *triangulateButton;
    QPushButton *triangulate_ppButton;
    QPushButton *triangulate_p2tButton;

friend MainWindow;   //需要友元，为了让主窗口使用
};

#endif // DOCKWIDGET_H

