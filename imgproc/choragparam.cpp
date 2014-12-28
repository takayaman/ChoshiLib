#include "choragparam.h"
#include "imgprocutil.h"

namespace choshiimgproc{

ChoRAGParam::ChoRAGParam( BaseChoImgProcParam *param )
    : BaseChoImgProcParam( param->getSrcImage(), param->getDstImage() )
{
    if( NULL != param->getResource() ){
        m_resource = param->getResource();
        m_choshiImage = param->getChoshiImage();
        m_clusterList = m_resource->getClusterList();
    }
    m_sRadius = 0;
    m_cRadius = 0;
    m_minPixNum = 100;
}

ChoRAGParam::~ChoRAGParam()
{
    m_srcImage = NULL;
    m_dstImage = NULL;
    m_choshiImage = NULL;
}

void ChoRAGParam::setSpatialRadius( float sRadius )
{
    m_sRadius = sRadius;
}

double ChoRAGParam::getSpatialRadius()
{
    return m_sRadius;
}

void ChoRAGParam::setColorRadius( float cRadius )
{
    m_cRadius = cRadius;
}

double ChoRAGParam::getColorRadius()
{
    return m_cRadius;
}

void ChoRAGParam::setCvTermCriteria( cv::TermCriteria termCriteria )
{
    m_termCriteria = termCriteria;
}

cv::TermCriteria ChoRAGParam::getCvTermCriteria()
{
    return m_termCriteria;
}

void ChoRAGParam::setMinPixNum( int minPixNum )
{
    m_minPixNum = minPixNum;
}

choshiresource::ChoCluster* ChoRAGParam::getClusterAt( int labelNum )
{
    return m_resource->getClusterAt( labelNum );
}

bool ChoRAGParam::deleteClusterAt( int labelNum )
{
    return m_resource->deleteClusterAt( labelNum );
}

int ChoRAGParam::getClusterSize()
{
    return m_clusterList->size();
}

cv::Vec2i ChoRAGParam::mergeBasedOnColor( choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1 )
{
    float diffColor = 0;
    ImgProcColorMode eMode = getColorMode();

    cv::Vec2i retVal;
    retVal.val[0] = -1;
    retVal.val[1] = -1;

    cv::Vec3d color0;
    cv::Vec3d color1;

    switch(eMode){
    case COLORMODE_RGB:
    {
        color0 = cluster0->m_meanRGB;
        color1 = cluster1->m_meanRGB;
        break;
    }
    case COLORMODE_LAB:
    {
        color0 = cluster0->m_meanLAB;
        color1 = cluster1->m_meanLAB;
        break;
    }
    case COLORMODE_HVC:
    {
        color0 = cluster0->m_meanHVC;
        color1 = cluster1->m_meanHVC;
        break;
    }
    default:
        return retVal;
    }

    diffColor = ImgProcUtil::calculateDiff( color0, color1, eMode );

    if( diffColor < getColorRadius() )
        retVal = merge( cluster0, cluster1 );

    return retVal;
}

cv::Vec2i ChoRAGParam::mergeBasedOnPixNum( choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1 )
{
    cv::Vec2i retVal;
    retVal.val[0] = -1;
    retVal.val[1] = -1;
    if( cluster0->m_holdPixelNum > m_minPixNum && cluster1->m_holdPixelNum > m_minPixNum )
        return retVal;

    ImgProcColorMode eMode = getColorMode();

    cv::Vec3d color0;
    cv::Vec3d color1;

    switch(eMode){
    case COLORMODE_RGB:
    {
        color0 = cluster0->m_meanRGB;
        color1 = cluster1->m_meanRGB;
        break;
    }
    case COLORMODE_LAB:
    {
        color0 = cluster0->m_meanLAB;
        color1 = cluster1->m_meanLAB;
        break;
    }
    case COLORMODE_HVC:
    {
        color0 = cluster0->m_meanHVC;
        color1 = cluster1->m_meanHVC;
        break;
    }
    default:
        return retVal;
    }

    /* check difference of color is lowest */
    double diffColor0 = ImgProcUtil::calculateDiff( color0, color1, eMode );
    double diffColor1 = 0;
    std::list<choshiresource::ChoCluster*>::iterator it = cluster0->m_adjacencyClusterList.begin();
    while( it != cluster0->m_adjacencyClusterList.end() ){
        choshiresource::ChoCluster *adjCluster = *it;
        switch(eMode){
        case COLORMODE_RGB:
        {
            color1 = adjCluster->m_meanRGB;
            break;
        }
        case COLORMODE_LAB:
        {
            color1 = adjCluster->m_meanLAB;
            break;
        }
        case COLORMODE_HVC:
        {
            color1 = adjCluster->m_meanHVC;
            break;
        }
        default:
            break;
        }
        diffColor1 = ImgProcUtil::calculateDiff( color0, color1, eMode );
        if( diffColor0 > diffColor1 )
            return retVal;
        it++;
    }

    retVal = merge( cluster0, cluster1 );
    return retVal;
}

cv::Vec2i ChoRAGParam::merge( choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1 )
{
    cv::Vec2i retVal;
    if( cluster0->m_holdPixelNum >= cluster1->m_holdPixelNum ){
        cluster0->absorbCluster( cluster1 );
        retVal.val[0] = cluster0->m_labelNum;
        retVal.val[1] = cluster1->m_labelNum;
    }
    else{
        cluster1->absorbCluster( cluster0 );
        retVal.val[0] = cluster1->m_labelNum;
        retVal.val[1] = cluster0->m_labelNum;
    }
    return retVal;
}

void ChoRAGParam::reassignLabelNum()
{
    m_resource->reassignLabelNum();;
}

} /* namespace ChoRAGParam */
