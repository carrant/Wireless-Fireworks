#-------------------------------------------------
#
# Project created by QtCreator 2016-06-23T23:25:25
#
#-------------------------------------------------

QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FireworksGuiAndroid
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ManagerSlat.cpp \
    SerialCommand.cpp \
    utils.cpp \
    FireShow.cpp \
    RBLBluetooth.cpp

HEADERS  += mainwindow.h \
    linkedlist.h \
    ManagerSlat.h \
    Messages.h \
    SerialCommand.h \
    utils.h \
    HardwareSerial.h \
    FireShow.h \
    RBLBluetooth.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    resources.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

