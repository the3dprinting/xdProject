#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "xdlib/readslice.h"
#include "centralwidget.h"
#include "dockwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void changeLayerNumRange(int n);  //用来改变dockwidget层数范围的信号

private slots:
    void on_actionOpen_triggered();
    void on_sliceButton_clicked();

private:
    Ui::MainWindow *ui;
    xd::TriangleMesh *triangleMesh;
    std::vector<xd::ExPolygons>* layers;
    centralwidget *cw;
    dockwidget *dw;
};

#endif // MAINWINDOW_H
