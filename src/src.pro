TEMPLATE = lib
CONFIG += static
TARGET = AllLib
#QT     -= gui
#CONFIG += c++11
#LIBS   -= -lQtGui -lGL

DESTDIR = $$OUT_PWD/../bin

HEADERS += \
    ./clipper/clipper.hpp


SOURCES += \
    ./clipper/clipper.cpp

INCLUDEPATH +=\
    $$PWD/clipper
