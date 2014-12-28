/******************************************************************************
 * Project      : ChoResource
 * File         : chocluster.cpp
 * Copyright    : 2013 (c) Natsuki Takayama, UEC. Tokyo
 * The source file of ChoCluster class.
 *****************************************************************************/

#include "chocluster.h"
#include "QString"

namespace choshiresource {

/********************************************************************
 * Constructor/Destructor
 *******************************************************************/

ChoCluster::ChoCluster()
{
    m_meanXY.zeros();
    m_meanRGB.zeros();
    m_meanHVC.zeros();
    m_meanLAB.zeros();
    m_holdPixelNum = 0;
    m_labelNum = 0;
}

ChoCluster::ChoCluster( ChoCluster *cluster )
{
    m_meanXY = cluster->m_meanXY;
    m_meanRGB = cluster->m_meanRGB;
    m_meanHVC = cluster->m_meanHVC;
    m_meanLAB = cluster->m_meanLAB;
    m_holdPixelNum = cluster->m_holdPixelNum;
    m_labelNum = cluster->m_labelNum;
    cluster->copyAdjacencyLabelNum( &m_adjacencyLabelNums );
    m_paperName = cluster->m_paperName;
}

ChoCluster::~ChoCluster()
{
    m_adjacencyClusterList.clear();
    m_adjacencyPointList.clear();
}

/********************************************************************
 * Handling adjacency clusters
 *******************************************************************/

/* Add other cluster as adjacency cluster */
void ChoCluster::addAdjacencyCluster( ChoCluster *node )
{
    /* do not add self as adjacency */
    if( this->m_labelNum == node->m_labelNum )
        return;
    if( NULL == seekAdjacencyCluster( node->m_labelNum ) )
        m_adjacencyClusterList.push_back( node );
}

/*-------------------------------------------------------*/

/* Delete adjacency cluster which is pointed by labelNum */
void ChoCluster::deleteAdjacencyCluster( int labelNum )
{
    ChoCluster *cluster = NULL;
    if( 0 == m_adjacencyClusterList.size() )
        return;
    std::list<ChoCluster *>::iterator it = m_adjacencyClusterList.begin();
    while( it != m_adjacencyClusterList.end() ){
        cluster = *it;
        if( labelNum == cluster->m_labelNum ){
            m_adjacencyClusterList.erase( it );
            it--; /* adjust index */
        }
        it++;
    }
}

void ChoCluster::clearAdjacencyClusterList()
{
    m_adjacencyClusterList.clear();
}

/*-------------------------------------------------------*/

/* Seek and return adjacency cluster which is pointed by labelNum */
ChoCluster* ChoCluster::seekAdjacencyCluster( int labelNum )
{
    ChoCluster *cluster = NULL;
    if( 0 == m_adjacencyClusterList.size() ) return NULL;
    std::list<ChoCluster *>::iterator it = m_adjacencyClusterList.begin();
    while( it != m_adjacencyClusterList.end() ){
        cluster = *it;
        if( labelNum == cluster->m_labelNum )
            return cluster;
        it++;
    }
    return NULL;
}

/*-------------------------------------------------------*/

/* Absorb other cluster into self */
bool ChoCluster::absorbCluster( ChoCluster *cluster )
{
    if( !cluster )
        return false;
    /* include holding pixel number and adjacency list */
    ChoCluster *adjacencyCluster;
    cv::Vec3d newColor;

    /* assign new feature value, weighted average  */
    double ratio = (double)this->m_holdPixelNum / (double)( this->m_holdPixelNum + cluster->m_holdPixelNum );
    this->m_meanHVC = this->m_meanHVC * ratio + cluster->m_meanHVC * ( 1 - ratio );
    this->m_meanLAB = this->m_meanLAB * ratio + cluster->m_meanLAB * ( 1 - ratio );
    for( int i = 0; i < 2; i++ ){
        double temp = (double)this->m_meanXY.val[i] * ratio + (double)cluster->m_meanXY.val[i] * ( 1 - ratio );
        this->m_meanXY.val[i] = (int)floor( temp + 0.5 );
    }
    for( int i = 0; i < 3; i++ ){
        double temp = (double)this->m_meanRGB.val[i] * ratio + (double)cluster->m_meanRGB.val[i] * ( 1 - ratio );
        this->m_meanRGB.val[i] = (int)floor( temp + 0.5 );
    }


    this->m_holdPixelNum += cluster->m_holdPixelNum;
    deleteAdjacencyCluster( cluster->m_labelNum );
    std::list<ChoCluster *>::iterator it = cluster->m_adjacencyClusterList.begin();
    while( it != cluster->m_adjacencyClusterList.end() ){
        adjacencyCluster = *it;
        adjacencyCluster->deleteAdjacencyCluster( cluster->m_labelNum );
        addAdjacencyCluster( adjacencyCluster );
        adjacencyCluster->addAdjacencyCluster( this );
        it++;
    }
    cluster->m_adjacencyClusterList.clear();
    return true;
}

/*-------------------------------------------------------*/

/* Create adjacency label number list
 * This function is designed for boost serialization(save data)
 */
void ChoCluster::createAdjacencyInfo()
{
    ChoCluster *cluster = NULL;
    std::list<ChoCluster *>::iterator it = m_adjacencyClusterList.begin();
    while( it != m_adjacencyClusterList.end() ){
        cluster = *it;
        int label = cluster->m_labelNum;
        m_adjacencyLabelNums.push_back( label );
        it++;
    }
}

/*-------------------------------------------------------*/

/* Check ittpt is adjacency point or not */
bool ChoCluster::checkAdjacencyPoint( cv::Point2i ittPt )
{
    std::list<cv::Point2i>::iterator it = this->m_adjacencyPointList.begin();
    while( it != this->m_adjacencyPointList.end() ){
        cv::Point2i point = *it;
        int diffX = abs( ittPt.x - point.x );
        int diffY = abs( ittPt.y - point.y );
        if( diffX <= 1 && diffY <= 1 )
            return true;
        it++;
    }
    return false;
}

/*-------------------------------------------------------*/

/* Copy adjacency label numbers */
void ChoCluster::copyAdjacencyLabelNum( std::vector<int> *adjacncyLabelNums )
{
    std::vector<int>::iterator it = m_adjacencyLabelNums.begin();
    while( it != m_adjacencyLabelNums.end() ){
        int temp = *it;
        adjacncyLabelNums->push_back( temp );
        it++;
    }
}

/*-------------------------------------------------------*/

bool ChoCluster::isIsolatedConvex( cv::Mat *choshiImg )
{
    cv::Mat mask = cv::Mat::zeros( choshiImg->size().height, choshiImg->size().width, CV_8UC1 );
    cv::Mat contours = cv::Mat::zeros( choshiImg->size().height, choshiImg->size().width, CV_8UC1 );

    /* make mask fo mass region */
    this->writeMask( choshiImg, &mask, this->m_labelNum );
    int debugCount = 0;
    QString debugFileName;
    debugFileName = QString( "./out/maskgrow%1.png").arg( debugCount );
    cv::imwrite( debugFileName.toStdString(), mask );
    debugCount++;

    std::list<ChoCluster *>::iterator it = m_adjacencyClusterList.begin();
    while( it != m_adjacencyClusterList.end() ){
        ChoCluster *cluster = *it;
        cluster->writeMask( choshiImg, &mask, this->m_labelNum );
        // Debug
        debugFileName = QString("./out/maskgrow%1.png").arg(debugCount);
        cv::imwrite(debugFileName.toStdString(), mask);
        it++;
        debugCount++;
    }

    /* make contour of mass */
    this->writeContour( &mask, &contours );

    /* Debug */
    debugFileName = QString( "./out/contourmasc%1.png" ).arg( this->m_labelNum );
    cv::imwrite( debugFileName.toStdString(), contours );

    cv::Mat maskSelf = cv::Mat::zeros( choshiImg->size().height, choshiImg->size().width, CV_8UC1 );
    cv::Mat contoursSelf = cv::Mat::zeros( choshiImg->size().height, choshiImg->size().width, CV_8UC1 );

    this->writeMask( choshiImg, &maskSelf, this->m_labelNum );
    this->writeContour( &maskSelf, &contoursSelf );

    /* Debug */
    debugFileName = QString( "./out/maskself%1.png" ).arg( this->m_labelNum );
    cv::imwrite( debugFileName.toStdString(), maskSelf );
    debugFileName = QString( "./out/contourself%1.png" ).arg( this->m_labelNum );
    cv::imwrite( debugFileName.toStdString(), contoursSelf );

    /* search common edges */
    for( int y = 0; y < choshiImg->size().height; y++ ){
        for( int x = 0; x < choshiImg->size().width; x++ ){
            int val0 = contours.at<uchar>( y, x );
            int val1 = contoursSelf.at<uchar>( y, x );
            if( 255 == val0 && 255 == val1 )
                return false;
        }
    }

    /* check this cluster is front row */
    it = m_adjacencyClusterList.begin();
    while( it != m_adjacencyClusterList.end() ){
        ChoCluster *cluster = *it;
        int labelNum = cluster->m_labelNum;
        if( labelNum < this->m_labelNum )
            return false;
        it++;
    }
    return true;
}

void ChoCluster::writeMask( cv::Mat *srcChoshiImg, cv::Mat *dstMaskImg, int labelNum )
{
    if( !srcChoshiImg->data || !dstMaskImg->data ){
        // TODO DebugMessage
        return;
    }

    for( int y = 0; y < srcChoshiImg->size().height; y++ ){
        for( int x = 0; x < srcChoshiImg->size().width; x++ ){
            cv::Vec6d tempVal = srcChoshiImg->at<cv::Vec6d>( y, x );
            // TODO make common CHANNEL MACRO
            if( labelNum == tempVal.val[srcChoshiImg->channels() - 1] || dstMaskImg->at<uchar>( y, x ) == 255 ){
                dstMaskImg->at<uchar>( y, x ) = 255;
            }else{
                dstMaskImg->at<uchar>( y, x ) = 0;
            }
        }
    }
}

void ChoCluster::writeContour( cv::Mat *srcMaskImg, cv::Mat *dstContourImg )
{
    /* find countour of mass */
    for( int y = 0; y < srcMaskImg->size().height; y++ ){
        for( int x = 0; x < srcMaskImg->size().width; x++ ){
            /* ignore when pixel is not white */
            if( 255 != srcMaskImg->at<uchar>( y, x ) )
                continue;

            /* border of image is contour */
            if( 0 == y || ( srcMaskImg->size().height - 1 == y ) || 0 == x || srcMaskImg->size().width - 1 == x ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }

            /* search 8-neighbers */
            /* top-left */
            if( 0 == srcMaskImg->at<uchar>( y - 1, x - 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* top-middle */
            if( 0 == srcMaskImg->at<uchar>( y - 1, x) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* top-right */
            if( 0 == srcMaskImg->at<uchar>( y - 1, x + 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* middle-left */
            if( 0 == srcMaskImg->at<uchar>( y, x - 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* middle-right */
            if( 0 == srcMaskImg->at<uchar>( y, x + 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* bottom-left */
            if( 0 == srcMaskImg->at<uchar>( y + 1, x - 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* bottom-middle */
            if( 0 == srcMaskImg->at<uchar>( y + 1, x) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* bottom-right */
            if( 0 == srcMaskImg->at<uchar>( y + 1, x + 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
        }
    }
}

} /* namespace choshiresource */
