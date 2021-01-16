#-------------------------------------------------
#
# Project created by QtCreator 2020-12-03T16:16:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PNQuads
TEMPLATE = app


SOURCES += main.cpp\
    bsplinerenderer.cpp \
        mainwindow.cpp \
    meshrenderer.cpp \
    objfile.cpp \
    mesh.cpp \
    meshtools.cpp \
    mainview.cpp \
    pnquadrenderer.cpp \
    settings.cpp

HEADERS  += mainwindow.h \
    bsplinerenderer.h \
    mesh.h \
    meshrenderer.h \
    objfile.h \
    patch.h \
    pnquadrenderer.h \
    renderer.h \
    settings.h \
    vertex.h \
    halfedge.h \
    face.h \
    mainview.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
