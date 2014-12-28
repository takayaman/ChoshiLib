#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T12:01:27
#
#-------------------------------------------------

QT       -= gui

TARGET = ChoImgProc

TEMPLATE = lib

DEFINES += CHOIMGPROC_LIBRARY

SOURCES += choimgproc.cpp \
    ichoimgproc.cpp \
    basechoimgprocparam.cpp \
    chomeanshift.cpp \
    chomeanshiftparam.cpp \
    imgprocutil.cpp \
    cholabeling.cpp \
    chorag.cpp \
    cholabelingparam.cpp \
    choragparam.cpp \
    choselectpapersparam.cpp \
    choselectpapers.cpp

HEADERS += choimgproc.h\
    choimgproc_global.h \
    ichoimgproc.h \
    basechoimgprocparam.h \
    chomeanshift.h \
    chomeanshiftparam.h \
    imgprocutil.h \
    basedef.h \
    cholabeling.h \
    chorag.h \
    cholabelingparam.h \
    choragparam.h \
    choselectpapersparam.h \
    choselectpapers.h \
    basepaperdatas.h

LIBS += `pkg-config opencv --libs` \
    -L"/usr/lib" \
    -L"/usr/lib/x86_64-linux-gnu" \
    -lglog

INCLUDEPATH += /home/takayaman/opencv3/build/include \
    /home/takayaman/Documents/Programming/choshi/ChoLib/imgres/ \
    /usr/include

unix:debug {
    LIBS += -L"/home/takayaman/Documents/Programming/choshi/ChoLib/buildLibs/debug" \
        -lChoResource
}
unix:release {
    LIBS += -L"/home/takayaman/Documents/Programming/choshi/ChoLib/buildLibs/release" \
        -lChoResource
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

QMAKE_CXXFLAGS += -g
