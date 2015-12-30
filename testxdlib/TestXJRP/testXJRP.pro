TARGET = testXJRP
TEMPLATE = app
QT       += core gui
QT       += opengl
QT       += widgets

CONFIG += c++11

DESTDIR = $$OUT_PWD/../../bin

unix|win32: LIBS += -L$$DESTDIR -lAllLib

INCLUDEPATH += $$PWD/../../src

DEPENDPATH += $$INCLUDEPATH

HEADERS += \
    draw.h \
    window.h

SOURCES += \
    draw.cpp \
    testinfill.cpp \
    window.cpp
