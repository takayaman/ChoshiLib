#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T11:22:14
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testPaperArrangeMent
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    choglwindowbase.cpp \
    imagewindow.cpp \
    perspectivewindow.cpp \
    choguiutil.cpp \
    chocstmodel.cpp \
    chocstitem.cpp

HEADERS  += mainwindow.h \
    choglwindowbase.h \
    imagewindow.h \
    perspectivewindow.h \
    choguibasedef.h \
    choguiutil.h \
    chocstmodel.h \
    chocstitem.h

LIBS += `pkg-config opencv --libs` \
    -L"/usr/lib" -lGLU \
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

FORMS    += mainwindow.ui
