#-------------------------------------------------
#
# Project created by QtCreator 2019-06-13T16:29:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ViewDiff
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagewin.cpp \
    imagelib.cpp

HEADERS  += mainwindow.h \
    imagewin.h \
    imagelib.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
