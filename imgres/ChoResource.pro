#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T14:12:05
#
#-------------------------------------------------

QT       -= gui

TARGET = ChoResource

TEMPLATE = lib

DEFINES += CHORESOURCE_LIBRARY

SOURCES += choresource.cpp \
    chocluster.cpp

HEADERS += choresource.h\
        choresource_global.h \
    chocluster.h

LIBS += `pkg-config opencv --libs` \
    -L"/usr/lib" -lboost_regex -lboost_serialization \
    -L"/usr/lib/x86_64-linux-gnu" \
    -lglog

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

INCLUDEPATH += /home/takayaman/opencv3/build/include \
    /usr/include

QMAKE_CXXFLAGS += -g
