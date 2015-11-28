TEMPLATE = lib
CONFIG += static
TARGET = AllLib
#QT     -= gui
#CONFIG += c++11
#LIBS   -= -lQtGui -lGL

DESTDIR = $$OUT_PWD/../bin

HEADERS += \    
    ./admesh/stl.h\
    ./clipper/clipper.hpp\
    ./poly2tri/poly2tri.h\
    ./polypartition/polypartition.h\
    poly2tri/sweep/advancing_front.h \
    poly2tri/sweep/cdt.h \
    poly2tri/sweep/sweep.h \
    poly2tri/sweep/sweep_context.h \
    poly2tri/common/shapes.h \
    poly2tri/common/utils.h \
    xdlib/point.h \
    xdlib/constdefine.h \
    xdlib/line.h \
    xdlib/multipoint.h


SOURCES += \
    admesh/connect.c \
    admesh/normals.c \
    admesh/shared.c \
    admesh/stl_io.c \
    admesh/stlinit.c \
    admesh/util.c \
    clipper/clipper.cpp \
    poly2tri/sweep/advancing_front.cc \
    poly2tri/sweep/cdt.cc \
    poly2tri/sweep/sweep.cc \
    poly2tri/sweep/sweep_context.cc \
    poly2tri/common/shapes.cc \
    polypartition/polypartition.cpp \
    xdlib/point.cpp



INCLUDEPATH +=\
    $$PWD/clipper
