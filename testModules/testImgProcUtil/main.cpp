
#include <glog/logging.h>
#include <QCoreApplication>
#include "opencv2/opencv.hpp"
#include "imgprocutil.h"
#include "choresource.h"
#include "choimgproc.h"
#include "chomeanshift.h"
#include "chomeanshiftparam.h"
#include "cholabeling.h"
#include "cholabelingparam.h"
#include "chorag.h"
#include "choragparam.h"
#include "choselectpapers.h"
#include "choselectpapersparam.h"
#include "ichoimgproc.h"
#include "QDebug"

using namespace choshiimgproc;
using namespace choshiresource;


void testImgProc( QCoreApplication *a, int argc, char* argv[] );
void testColorConvert(QCoreApplication *a );
void testHVCPaper();
void testLABPaper();

cv::Mat* img;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;

    //testHVCPaper();
    //testLABPaper();

    //exit(0);
    testImgProc(&a, argc, argv);

    google::InstallFailureSignalHandler();

    return a.exec();

}

void testHVCPaper(){
    BaseChoImgProcParam *baseParam = new BaseChoImgProcParam( NULL, NULL );
    ChoResource *resource = new choshiresource::ChoResource();
    baseParam->setResource( resource );
    ChoSelectPapersParam *selectionParam = new ChoSelectPapersParam( baseParam );
    selectionParam->setUsePapers( JISZ8102 );
    selectionParam->setColorMode( choshiimgproc::COLORMODE_HVC );

    int minPapers[268];
    double minDiffs[268];
    for( int i = 0; i < 268; i++ ){
        minPapers[i] = -1;
        minDiffs[i] = 100000;
    }

    for( int i = 0; i < 268; i++ ){
        cv::Vec3d color0;
        color0 = selectionParam->getPaperHVC( i );

        for( int j = 0; j < 268; j++ ){
            if( i == j )
                continue;
            cv::Vec3d color1;
            color1 = selectionParam->getPaperHVC( j );
            double diff;
            diff = ImgProcUtil::calculateDiff( color0, color1, choshiimgproc::COLORMODE_HVC );
            if( diff == 0 )
                continue;
            if( minDiffs[i] > diff ){
                minDiffs[i] = diff;
                minPapers[i] = j;
            }
        }
        std::string debug = selectionParam->getPaperName( minPapers[i] );
    }

    double maxDiff = 0;
    cv::Vec2i maxPaper = cv::Vec2i(-1, -1);
    for( int i = 0; i < 268; i++ ){
        if( maxDiff < minDiffs[i] ){
            maxDiff = minDiffs[i];
            maxPaper.val[0] = i;
            maxPaper.val[1] = minPapers[i];
        }
    }

    std::string paperName0 = selectionParam->getPaperName( maxPaper.val[0] );
    std::string paperName1 = selectionParam->getPaperName( maxPaper.val[1] );
    double diff = minDiffs[maxPaper.val[0]];

    std::cout << "paper0:" << paperName0 << ",paper1:" << paperName1 << ",diff:" << diff;

    delete selectionParam;
    delete baseParam;
    delete resource;
}

void testLABPaper(){
    BaseChoImgProcParam *baseParam = new BaseChoImgProcParam( NULL, NULL );
    ChoResource *resource = new choshiresource::ChoResource();
    baseParam->setResource( resource );
    ChoSelectPapersParam *selectionParam = new ChoSelectPapersParam( baseParam );
    selectionParam->setUsePapers( JISZ8102 );
    selectionParam->setColorMode( choshiimgproc::COLORMODE_LAB );

    int minPapers[268];
    double minDiffs[268];
    for( int i = 0; i < 268; i++ ){
        minPapers[i] = -1;
        minDiffs[i] = 100000;
    }

    for( int i = 0; i < 268; i++ ){
        cv::Vec3d color0;
        color0 = selectionParam->getPaperLAB( i );

        for( int j = 0; j < 268; j++ ){
            if( i == j )
                continue;
            cv::Vec3d color1;
            color1 = selectionParam->getPaperLAB( j );
            double diff;
            diff = ImgProcUtil::calculateDiff( color0, color1, choshiimgproc::COLORMODE_LAB );
            if( diff == 0 )
                continue;
            if( minDiffs[i] > diff ){
                minDiffs[i] = diff;
                minPapers[i] = j;
            }
        }
        std::string debug = selectionParam->getPaperName( minPapers[i] );
    }

    double maxDiff = 0;
    cv::Vec2i maxPaper = cv::Vec2i(-1, -1);
    for( int i = 0; i < 268; i++ ){
        if( maxDiff < minDiffs[i] ){
            maxDiff = minDiffs[i];
            maxPaper.val[0] = i;
            maxPaper.val[1] = minPapers[i];
        }
    }

    std::string paperName0 = selectionParam->getPaperName( maxPaper.val[0] );
    std::string paperName1 = selectionParam->getPaperName( maxPaper.val[1] );
    double diff = minDiffs[maxPaper.val[0]];

    std::cout << "paper0:" << paperName0 << ",paper1:" << paperName1 << ",diff:" << diff;

    delete selectionParam;
    delete baseParam;
    delete resource;
}

void testImgProc( QCoreApplication *a, int argc, char* argv[] ){

    if(argc <= 1){
        std::cout << "param error: Program imgfile";
        return;
    }

    float sRadMean = 20;
    float cRadMean = 30;
    //float sRadLab = 10;
    //float cRadLab = 15;
    float sRadRAG = 20;
    float cRadRAG = 30;
    //choshiimgproc::ImgProcColorMode ColorMode = choshiimgproc::COLORMODE_RGB;
    //choshiimgproc::ImgProcColorMode ColorMode = choshiimgproc::COLORMODE_LAB;
    choshiimgproc::ImgProcColorMode ColorMode = choshiimgproc::COLORMODE_HVC;

    if(  choshiimgproc::COLORMODE_RGB == ColorMode ){
        cRadMean = 30;
        cRadRAG = 30;
    }else if( choshiimgproc::COLORMODE_LAB == ColorMode ){
        cRadMean = 20;
        cRadRAG = 20;
    }else if( choshiimgproc::COLORMODE_HVC == ColorMode ){
        cRadMean = 2;
        cRadRAG = 2;
    }

    cv::TermCriteria termCrit;
    termCrit.maxCount = 5;
    termCrit.epsilon = 1.0;
    termCrit.type = cv::TermCriteria::COUNT + cv::TermCriteria::EPS;
    std::string outDir;

    if( argc >= 3 ) outDir = argv[2];
    else outDir = "./out";
    if(argc >= 4)   sRadMean = atof(argv[3]);
    if(argc >= 5)   cRadMean = atof(argv[4]);
    if(argc >= 6)   termCrit.maxCount = atoi(argv[5]);
    //if(argc >= 7)   sRadLab = atof(argv[6]);
    //if(argc >= 8)   cRadLab = atof(argv[7]);
    if(argc >= 7)   sRadRAG = atof(argv[6]);
    if(argc >= 8)  cRadRAG = atof(argv[7]);
    if(argc >= 9) ColorMode = (ImgProcColorMode)atof(argv[8]);

    cv::Mat inputImg = cv::imread( argv[1] );

    ChoImgProc *proc = new ChoImgProc();
    proc->setImage( &inputImg );

    ChoResource *resource = new ChoResource();
    BaseChoImgProcParam *baseParam = new BaseChoImgProcParam( proc->getInputImage(), proc->getDispImage() );
    resource->createChoshiImage( proc->getInputImage() );
    baseParam->setResource( resource );

    qDebug() << a->tr("setup finish\n");

    // meanshift
    ChoMeanShift *mShift = new ChoMeanShift();
    IChoImgProc *iProc = mShift;
    proc->setImgProc( iProc );
    ChoMeanShiftParam *mShiftParam = new ChoMeanShiftParam( baseParam );
    mShiftParam->setColorRadius( cRadMean );
    mShiftParam->setSpatialRadius( sRadMean );
    mShiftParam->setCvTermCriteria( termCrit );
    if( choshiimgproc::COLORMODE_RGB == ColorMode )
        mShiftParam->setColorMode( choshiimgproc::COLORMODE_RGB );
    else if( choshiimgproc::COLORMODE_HVC == ColorMode )
        mShiftParam->setColorMode( choshiimgproc::COLORMODE_HVC );
    else if( choshiimgproc::COLORMODE_LAB == ColorMode )
        mShiftParam->setColorMode( choshiimgproc::COLORMODE_LAB );

    BaseChoImgProcParam *bParam = mShiftParam;
    proc->init( bParam );
    if( choshiimgproc::COLORMODE_HVC == ColorMode )
        proc->convertColor( RGB2HVC );
    else if( choshiimgproc::COLORMODE_LAB == ColorMode )
        proc->convertColor( RGB2LAB );

    proc->execute();
    proc->createDstImage();

    img = proc->getDispImage();
    cv::Mat saveImg = img->clone();
    std::string outPath = outDir;
    outPath += "/meanshift.png";
    cv::imwrite( outPath, saveImg );

     qDebug() << a->tr("meanshift finish\n");

    // labering
    ChoLabeling *labeling = new ChoLabeling();
    iProc = labeling;
    proc->setImgProc( iProc );
    ChoLabelingParam *labelParam = new ChoLabelingParam( baseParam );
    labelParam->setColorRadius( mShiftParam->getColorRadius() / 2 );
    //labelParam->setColorRadius( 2 );
    labelParam->setColorMode( mShiftParam->getColorMode() );
    bParam = labelParam;
    proc->init( bParam );
    proc->execute();

    img = proc->getDispImage();
    saveImg = img->clone();
    outPath = outDir;
    outPath += "/labeling.png";
    cv::imwrite( outPath, saveImg );

     qDebug() << a->tr("labeling finish\n");

    // rag
    ChoRAG *rag = new ChoRAG();
    iProc = rag;
    proc->setImgProc( iProc );
    ChoRAGParam *ragParam = new ChoRAGParam( baseParam );
    ragParam->setSpatialRadius( sRadRAG / 2 );
    ragParam->setColorRadius( cRadRAG / 2 );
    ragParam->setColorMode( mShiftParam->getColorMode() );

    ragParam->setCvTermCriteria( mShiftParam->getCvTermCriteria() );
    bParam = ragParam;
    proc->init( bParam );
    proc->execute();

    img = proc->getDispImage();
    saveImg = img->clone();
    outPath = outDir;
    outPath += "/rag0.png";
    cv::imwrite( outPath, saveImg );

     qDebug() << a->tr("make RAG and region merge\n");

    // select papers
    ChoSelectPapers *selection = new ChoSelectPapers();
    iProc = selection;
    proc->setImgProc( iProc );
    ChoSelectPapersParam *selectionParam = new ChoSelectPapersParam( baseParam );
    selectionParam->setUsePapers( JISZ8102 );
    selectionParam->setColorMode( mShiftParam->getColorMode() );
    bParam = selectionParam;
    proc->init( bParam );
    proc->execute();

    img = proc->getDispImage();
    saveImg = img->clone();
    outPath = outDir;
    outPath += "/selectPapers.png";
    cv::imwrite( outPath, saveImg );

    qDebug() << a->tr("color change finish\n");

    // rag again
    iProc = rag;
    proc->setImgProc( iProc );
    bParam = ragParam;
    proc->init( bParam );
    proc->execute();

    img = proc->getDispImage();
    saveImg = img->clone();
    outPath = outDir;
    outPath += "/rag1.png";
    cv::imwrite( outPath, saveImg );

    qDebug() << a->tr("second region merge\n");

    // cluster copy
    resource->copyColorChannelImage(img);
    saveImg = img->clone();
    outPath = outDir;
    outPath +="/cluster.png";
    cv::imwrite( outPath, saveImg );

    qDebug() << a->tr("cluster png output\n");

    // save
    resource->setResourcePath( outDir );
    resource->save();
    resource->clear();

    delete selection;
    delete rag;
    delete labeling;
    delete mShift;
    delete resource;
    delete proc;

    if( selectionParam )
        delete selectionParam;
    if( ragParam )
        delete ragParam;
    if( labelParam )
        delete labelParam;
    if( mShiftParam )
        delete mShiftParam;
    if( baseParam )
        delete baseParam;

    qDebug() << a->tr("finish!!\n");
}

void testColorConvert( QCoreApplication *a )
{
    DefColorAndWhite eMode = NTSC_C;

    double R = 2.0 / 255.0;
    double G = 8.0 / 255.0;
    double B = 143.0 / 255.0;
    cv::Vec3d RGB( R, G, B );

    cv::Vec3d HVC = ImgProcUtil::calculateMTM( RGB, eMode );
    cv::Vec3d LAB = ImgProcUtil::calculateLAB( RGB, eMode );
    //HVC = cv::Vec3d(327.054, 5.0565, 18.025);
    cv::Vec3d revRGB = ImgProcUtil::calculateReverseMTM( HVC, eMode );
    cv::Vec3b rgb8 = ImgProcUtil::convertIntegerRGB( revRGB );

    for( int r = 0; r < 256; r++ ){
        for( int g = 0; g < 256; g++ ){
            for( int b = 0; b < 256; b++ ){
                R = (double)r / 255.0;
                G = (double)g / 255.0;
                B = (double)b / 255.0;
                RGB = cv::Vec3d( R, G, B );
                HVC = ImgProcUtil::calculateMTM( RGB, eMode );
                LAB = ImgProcUtil::calculateLAB( RGB, eMode );
                revRGB = ImgProcUtil::calculateReverseMTM( HVC, eMode );
                if( -1 == revRGB.val[0] ){
                    qDebug() << a->tr("do not convergence r:%1, g:%2, b:%3").arg(r) .arg(g) .arg(b);
                    //return a.exec();
                }
                rgb8 = ImgProcUtil::convertIntegerRGB( revRGB );
                int diffR = abs(rgb8.val[0]-r);
                int diffG = abs(rgb8.val[1]-g);
                int diffB = abs(rgb8.val[2]-b);
                if( diffR >= 4 || diffG >= 4 || diffB >= 4 ){
                    qDebug() << a->tr("error too big r:%1, g:%2, b%3").arg(r) .arg(g) .arg(b);
                    //return a.exec();
                }
                revRGB = ImgProcUtil::calculateReverseLAB( LAB, eMode );
                rgb8 = ImgProcUtil::convertIntegerRGB( revRGB );
                diffR = abs(rgb8.val[0]-r);
                diffG = abs(rgb8.val[1]-g);
                diffB = abs(rgb8.val[2]-b);
                if( diffR >= 3 || diffG >= 3 || diffB >= 3 ){
                    qDebug() << a->tr("error too big lr%1, lg%2, lb%3").arg(r) .arg(g) .arg(b);
                }
            }
        }
    }
}
