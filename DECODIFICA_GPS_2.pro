#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T14:20:30
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DECODIFICA_GPS_2
TEMPLATE = app
CONFIG += c++11

QMAKE_CFLAGS_RELEASE += -O3 -mtune=generic -fomit-frame-pointer
QMAKE_CXXFLAGS_RELEASE += -O3 -mtune=generic -fomit-frame-pointer

SOURCES += main.cpp\
        janela.cpp

HEADERS  += janela.h

FORMS    += janela.ui

RESOURCES +=
