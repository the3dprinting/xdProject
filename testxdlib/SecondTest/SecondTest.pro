TARGET = SecondTest
TEMPLATE = app
QT       += core gui
QT       += opengl
QT       += widgets

DESTDIR = $$OUT_PWD/../../bin

unix|win32: LIBS += -L$$DESTDIR -lAllLib

INCLUDEPATH += $$PWD/../../src

DEPENDPATH += $$INCLUDEPATH

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    centralwidget.cpp \
    dockwidget.cpp \
    expolygonitem.cpp

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h \
    centralwidget.h \
    dockwidget.h \
    expolygonitem.h
