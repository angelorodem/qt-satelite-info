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

QMAKE_CFLAGS_RELEASE += -O3 -march=native -mtune=native -mfpmath=sse+387 -lstdc++
QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -mtune=native -mfpmath=sse+387 -lstdc++ # haswell
#QMAKE_CXXFLAGS_RELEASE +=  -Os -march=native -mtune=native -mfpmath=sse+387 -lstdc++#size  -fdata-sections -ffunction-sections file file.o -Wl,--gc-sections
#QMAKE_CXXFLAGS_RELEASE = -Os -Wl,-as-needed -Wl,-gc-sections -march=native -mtune=native -mfpmath=sse+387 -Wl,-dead_strip -Wl,-dead_strip_dylibs #size

SOURCES += main.cpp\
        janela.cpp

HEADERS  += janela.h

FORMS    += janela.ui

RESOURCES +=
