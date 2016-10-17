#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "centralwidget.h"
#include "dockwidget.h"
#include "xdlib/readslice.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QSpinBox;
class QPushButton;
class QLineEdit;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_sliceButton_clicked();
    void on_centerButton_clicked();
    void layerNumChanged(int i);

signals:
    void changeLayerNumRange(int n);  //用来改变dockwidget层数范围的信号


private:
    Ui::MainWindow *ui;
    centralwidget *cw;
    dockwidget *dw;
    xd::TriangleMesh *triangleMesh;
    std::vector<xd::ExPolygons>* layers;
};

#endif // MAINWINDOW_H
