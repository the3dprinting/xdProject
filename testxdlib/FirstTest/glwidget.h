#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWidget>
#include <QtOpenGL>

namespace Ui {
class GLwidget;
}

class GLwidget : public QWidget
{
    Q_OBJECT

public:
    explicit GLwidget(QWidget *parent = 0);
    ~GLwidget();

private:
    Ui::GLwidget *ui;
};

#endif // GLWIDGET_H
