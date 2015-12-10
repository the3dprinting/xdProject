#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "centralwidget.h"
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

private:
    Ui::MainWindow *ui;
    centralwidget *cw;
    xd::TriangleMesh *triangleMesh;
    QPushButton *testButton;

};

#endif // MAINWINDOW_H
