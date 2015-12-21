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
    xdlib/polyline.h \
    xdlib/geometry.h \
    xdlib/multiPoint.h \
    xdlib/polygon.h \
    xdlib/encapsulationClipper.h \
    xdlib/boundingBox.h \
    xdlib/exPolygon.h \
    xdlib/readSlice.h \
    xdlib/exPolygonGather.h \
    xdlib/planMotion.h \
    xdlib/polylineGather.h \
    xdlib/detectBridge.h \
    xdlib/surface.h \
    xdlib/surfaceGather.h \
    xdlib/config.h \
    xdlib/pringconfig.h


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
    xdlib/point.cpp \
    xdlib/line.cpp \
    xdlib/geometry.cpp \
    xdlib/polygon.cpp \
    xdlib/multiPoint.cpp \
    xdlib/polyline.cpp \
    xdlib/boundingBox.cpp \
    xdlib/encapsulationClipper.cpp \
    xdlib/exPolygon.cpp \
    xdlib/readSlice.cpp \
    xdlib/exPolygonGather.cpp \
    xdlib/planMotion.cpp \
    xdlib/polylineGather.cpp \
    xdlib/detectBridge.cpp \
    xdlib/surface.cpp \
    xdlib/surfaceGather.cpp \
    xdlib/config.cpp \
    xdlib/printConfig.cpp



INCLUDEPATH += $$PWD

DEPENDPATH += $$INCLUDEPATH
