#-------------------------------------------------
#
# Project created by QtCreator 2018-09-29T15:49:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Project
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QT += charts

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    homewidget.cpp \
    projecttree.cpp \
    subwindow.cpp \
    properties.cpp \
    projecttreemodel.cpp \
    model.cpp \
    world.cpp \
    item.cpp \
    vector2.cpp \
    projecttreeitem.cpp \
    connections.cpp \
    connectionsitem.cpp \
    connectionsmodel.cpp \
    point.cpp \
    display.cpp \
    screenitem.cpp \
    polygon.cpp \
    viewholder.cpp \
    circle.cpp \
    slider.cpp \
    force.cpp \
    graph.cpp \
    submodel.cpp

HEADERS += \
        mainwindow.h \
    homewidget.h \
    homewidget.h \
    projecttree.h \
    subwindow.h \
    properties.h \
    projecttreemodel.h \
    model.h \
    world.h \
    item.h \
    vector2.h \
    projecttreeitem.h \
    connections.h \
    connectionsitem.h \
    connectionsmodel.h \
    point.h \
    display.h \
    screenitem.h \
    polygon.h \
    viewholder.h \
    circle.h \
    slider.h \
    force.h \
    graph.h \
    submodel.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
