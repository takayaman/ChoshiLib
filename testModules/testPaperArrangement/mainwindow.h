/*=============================================================================
 * Project : TestPaperArrangement
 * Code : mainwindow.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 *
 *===========================================================================*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "imagewindow.h"
#include "perspectivewindow.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void quitAction();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionExit_triggered();

    void on_pushButtonUP_clicked();

    void on_pushButtonDOWN_clicked();

    void on_pushButtonLEFT_clicked();

    void on_pushButtonRIGHT_clicked();

    void on_pushButtonCHECKCONVEX_clicked();

    void drawCrossSectionOnSectionWindow( QPoint position );

    void drawSelectAreaOnImageWindow(QPoint position);

private:
    Ui::MainWindow *ui;

    choshigui::ImageWindow *m_imageWindow;
    choshigui::ImageWindow *m_crossSectionWindow;
    choshigui::PerspectiveWindow *m_perseWindow;

    cv::Mat m_crossSectionImage;
    cv::Mat m_overlayImage;

    QPoint m_sectionPosition;
    choshiresource::ChoResource m_resource;
    bool m_isResourceDeleted;

};

#endif // MAINWINDOW_H
