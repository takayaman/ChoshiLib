#-------------------------------------------------
#
# Project created by QtCreator 2014-01-20T19:38:19
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = testImgProcUtil
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

HEADERS += imgprocutil.h \
    choresource.h

LIBS += `pkg-config opencv --libs` \
    -L"/usr/lib" \
    -L"/usr/lib/x86_64-linux-gnu" \
    -lglog

unix:debug {
  LIBS += -L"/home/takayaman/Documents/Programming/choshi/ChoLib/buildLibs/debug" \
  -lChoImgProc -lChoResource
}
unix:release {
  LIBS += -L"/home/takayaman/Documents/Programming/choshi/ChoLib/buildLibs/release" \
  -lChoImgProc -lChoResource
}

INCLUDEPATH += /home/takayaman/opencv3/build/include \
    /home/takayaman/Documents/Programming/choshi/ChoLib/imgproc/ \
    /home/takayaman/Documents/Programming/choshi/ChoLib/imgres/ \
    /usr/include

