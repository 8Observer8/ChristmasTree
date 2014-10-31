#-------------------------------------------------
#
# Project created by QtCreator 2014-09-27T10:51:18
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChristmasTree
TEMPLATE = app


SOURCES += main.cpp\
        Dialog.cpp \
    Scene.cpp

HEADERS  += Dialog.h \
    Scene.h \
    Ground.h \
    Cube.h

FORMS    += Dialog.ui

RESOURCES += \
    Textures.qrc
