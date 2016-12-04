#-------------------------------------------------
#
# Project created by QtCreator 2016-06-27T23:47:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FireworkSimulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    slat.cpp \
    widgetslat.cpp \
    fireshow.cpp

HEADERS  += mainwindow.h \
    slat.h \
    widgetslat.h \
    fireshow.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    resources.qrc

