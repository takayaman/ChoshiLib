/*=============================================================================
 * Project : TestPaperArrangement
 * Code : main.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Entry point of TestPaperArrangement
 *===========================================================================*/

#include <glog/logging.h>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;

    MainWindow w;
    w.show();

    google::InstallFailureSignalHandler();
    return a.exec();
}
