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

QMAKE_CXXFLAGS_RELEASE -= -O2 -O1

#QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -mtune=native -mfpmath=sse+387 -lstdc++ # haswell
QMAKE_CXXFLAGS_RELEASE += -Os -mtune=generic -mfpmath=sse+387 -lstdc++

SOURCES += main.cpp\
        janela.cpp

HEADERS  += janela.h

FORMS    += janela.ui

