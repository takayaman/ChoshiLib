#include "cholabelingparam.h"

namespace choshiimgproc{

ChoLabelingParam::ChoLabelingParam( BaseChoImgProcParam *param )
    : BaseChoImgProcParam( param->getSrcImage(), param->getDstImage() )
{
    if( NULL != param->getResource() ){
        m_resource = param->getResource();
        m_choshiImage = param->getChoshiImage();
        m_clusterList = m_resource->getClusterList();
        m_chechAdjacency = false;
    }
    m_neighborsMode = EIGHT_NEIGHBORS;
}

ChoLabelingParam::~ChoLabelingParam()
{
    m_srcImage = NULL;
    m_dstImage = NULL;
    m_choshiImage = NULL;
}

void ChoLabelingParam::setColorRadius( float cRadius )
{
    m_cRadius = cRadius;
}

float ChoLabelingParam::getColorRadius( void )
{
    return m_cRadius;
}

void ChoLabelingParam::setNeighborsMode( Neighbours_mode eMode )
{
    m_neighborsMode = eMode;
}

Neighbours_mode ChoLabelingParam::getNeighborsMode()
{
    return m_neighborsMode;
}

void ChoLabelingParam::addCluster( choshiresource::ChoCluster *cluster )
{
    m_resource->addCluster( cluster );
}

} /* namespace choshiimgproc */
