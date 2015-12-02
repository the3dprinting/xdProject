#include "glwidget.h"
#include "ui_glwidget.h"
#include <gl/GLU.h>

GLwidget::GLwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GLwidget)
{
    ui->setupUi(this);
}

GLwidget::~GLwidget()
{
    delete ui;
}
