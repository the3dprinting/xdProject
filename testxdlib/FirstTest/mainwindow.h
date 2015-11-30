#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "xdlib/readslice.h"

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

private:
    Ui::MainWindow *ui;
    xd::TriangleMeshSlicer *Slicer;
};

#endif // MAINWINDOW_H
