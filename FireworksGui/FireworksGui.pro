#-------------------------------------------------
#
# Project created by QtCreator 2016-05-08T16:04:23
#
#-------------------------------------------------

QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FireworksGui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SerialCommand.cpp \
    utils.cpp \
    ManagerSlat.cpp \
    fireshow.cpp

HEADERS  += mainwindow.h \
    HardwareSerial.h \
    linkedlist.h \
    SerialCommand.h \
    utils.h \
    Messages.h \
    ManagerSlat.h \
    fireshow.h

FORMS    += mainwindow.ui \
    mainandroidwindow.ui

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

RESOURCES += \
    resources.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
