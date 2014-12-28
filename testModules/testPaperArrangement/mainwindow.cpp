/*=============================================================================
 * Project : TestPaperArrangement
 * Code : mainwindow.cpp
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Implementation of QMainwindow.
 * This window has responsibility of all controls ans resources.
 *===========================================================================*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include "imagewindow.h"
#include "perspectivewindow.h"

#define DEFAULTDIR "/home/takayaman/Documents/programs"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    m_imageWindow = NULL;
    m_perseWindow = NULL;
    m_crossSectionWindow = NULL;
    m_isResourceDeleted = true;
}

MainWindow::~MainWindow()
{
    if( !m_isResourceDeleted ){
        if( !m_imageWindow )
            delete m_imageWindow;
        if( !m_perseWindow )
            delete m_perseWindow;
        if( !m_crossSectionImage.data )
            m_crossSectionImage.release();
        if( !m_crossSectionWindow )
            delete m_crossSectionWindow;
        m_resource.clear();
    }

    delete ui;
}

void MainWindow::quitAction()
{
    if( !m_imageWindow )
        delete m_imageWindow;
    if( !m_perseWindow )
        delete m_perseWindow;
    if( !m_crossSectionImage.data )
        m_crossSectionImage.release();
    if( !m_crossSectionWindow )
        delete m_crossSectionWindow;
    m_resource.clear();
    m_isResourceDeleted = true;
}

void MainWindow::on_actionOpen_triggered()
{
    /*----------------------------------------*
     * Open file dialog and select file
     *----------------------------------------*/
    QStringList fileFilters;
    fileFilters += ( "XML (*.xml)" );
    QFileDialog *dir = new QFileDialog( this, tr( "Open File" ), DEFAULTDIR, QDir::currentPath() );
    dir->setNameFilters( fileFilters );
    dir->setAcceptMode( QFileDialog::AcceptOpen );

    QStringList fileNames;
    if( dir->exec() )
        fileNames = dir->selectedFiles();

    if( 0 == fileNames.size() )
        return;

    /* judge file extension */
    QString extension = fileNames.at(0).right(3);
    if( 1 == fileNames.size() ){
        /* load choshi file */
        if( 0 == extension.compare( "xml" ) ){
            QFileInfo *fileInfo = new QFileInfo( fileNames.at(0) );
            m_resource.setResourcePath( fileInfo->path().toStdString() );
            m_resource.load();
            m_isResourceDeleted = false;

            //cv::Mat dispImage = cv::Mat( m_resource.getImageHeight(), m_resource.getImageWidth(), CV_8UC3 );
            cv::Mat dispImage = cv::Mat(m_resource.getImageHeight(), m_resource.getImageWidth(), CV_8UC4);
            m_overlayImage = cv::Mat::zeros(m_resource.getImageHeight(), m_resource.getImageWidth(), CV_8UC4);
            //m_resource.copyColorChannelImage( &dispImage );
            m_resource.copyRGBAImage(&dispImage);
            cv::imwrite("./test0.png", dispImage);

            m_imageWindow = new choshigui::ImageWindow( 0, &dispImage );
            connect( m_imageWindow, SIGNAL(requestDrawCrossSection( QPoint )), this, SLOT( drawCrossSectionOnSectionWindow(QPoint)) );
            connect(m_imageWindow, SIGNAL(requestSelectAreaImage(QPoint)), this, SLOT(drawSelectAreaOnImageWindow(QPoint)));

            m_imageWindow->show();;
#if 1
            m_perseWindow = new choshigui::PerspectiveWindow( 0, &m_resource );
            m_perseWindow->show();
            m_perseWindow->setAnimating( true );

            m_crossSectionImage = cv::Mat::zeros( m_resource.getImageHeight(), m_resource.getImageWidth(), CV_8UC4 );
            m_resource.extractSectionImage( &m_crossSectionImage, m_resource.getImageHeight() / 2, choshiresource::SECTIONASSIDE_X );
            m_crossSectionWindow = new choshigui::ImageWindow( 0, &m_crossSectionImage ) ;
            m_crossSectionWindow->show();

            m_sectionPosition = QPoint( m_resource.getImageWidth() / 2, m_resource.getImageHeight() / 2 );
#endif
            delete fileInfo;
        }
    }
    delete dir;

    // set tree view;

}

void MainWindow::on_actionSave_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{
    quitAction();
    this->close();
}

void MainWindow::on_pushButtonUP_clicked()
{

}

void MainWindow::on_pushButtonDOWN_clicked()
{

}

void MainWindow::on_pushButtonLEFT_clicked()
{

}

void MainWindow::on_pushButtonRIGHT_clicked()
{

}

void MainWindow::on_pushButtonCHECKCONVEX_clicked()
{

}

void MainWindow::drawCrossSectionOnSectionWindow( QPoint position )
{
    if(0 > position.x())
        return;

    if( !m_crossSectionWindow )
        return;
    m_resource.extractSectionImage( &m_crossSectionImage, position.y(), choshiresource::SECTIONASSIDE_X );
    m_crossSectionWindow->setAnimating(true);
    m_crossSectionWindow->loadTexture( &m_crossSectionImage, choshigui::ID_IMAGE );
    m_crossSectionWindow->setAnimating(false);
    m_sectionPosition = position;
}

void MainWindow::drawSelectAreaOnImageWindow(QPoint position)
{
    if(0 > position.x())
            return;
    if(!m_imageWindow)
        return;
    cv::Point point = cv::Point(position.x(), position.y());
    m_resource.extractSelectArea(&m_overlayImage, point);
    m_imageWindow->loadTexture(&m_overlayImage, choshigui::ID_OVERLAY);
}
