#-------------------------------------------------
#
# Project created by QtCreator 2015-11-28T09:09:18
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FirstTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    centralwidget.cpp

HEADERS  += mainwindow.h \
    centralwidget.h

FORMS    += mainwindow.ui

DESTDIR = $$OUT_PWD/../../bin

unix|win32: LIBS += -L$$DESTDIR -lAllLib

INCLUDEPATH += $$PWD/../../src

DEPENDPATH += $$INCLUDEPATH
