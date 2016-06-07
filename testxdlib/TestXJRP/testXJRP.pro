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

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
