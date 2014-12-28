#include "chorag.h"
#include "imgprocutil.h"

namespace choshiimgproc {

ChoRAG::ChoRAG() {
  m_param = NULL;
}

ChoRAG::~ChoRAG() {
  if( !m_param )
    this->clear();
}

bool ChoRAG::init( BaseChoImgProcParam *param ) {
  /* Check class type */
  if( typeid( *param ) != typeid( ChoRAGParam ) ) {
    // TODO DebugMessage
    return false;
  }
  m_param = pointer_cast<ChoRAGParam*>( param );
  return true;
}

bool ChoRAG::clear() {
  if( !m_param )
    delete m_param;
  return true;
}

bool ChoRAG::execute() {
  if( !m_param ) {
    // TODO DebugMessage
    return false;
  }

  /* clear RAG */
  clearAdjacency();

  /* make RAG */
  makeAdjacency();

  /* fusion */
  cv::TermCriteria termCriteria = m_param->getCvTermCriteria();
  merge( ChoRAG::MARGE_BASED_COLOR, termCriteria );
  termCriteria.maxCount = 1;
  merge( ChoRAG::MARGE_BASED_PIXNUM, termCriteria );

  /* copy display Image */
  m_param->reflectClusterValToChoshiImage();
  createDstImage();

  return true;
}

void ChoRAG::makeAdjacency() {
  cv::Mat *choshiImage = m_param->getChoshiImage();
  int width = m_param->getImageWidth();
  int height = m_param->getImageHeight();

  cv::Point2i ittPoint;
  ittPoint.x = 0;
  ittPoint.y = 0;

  /* make adjacency */
  for( ittPoint.y = 1; ittPoint.y < height - 1; ittPoint.y++ ) {
    for( ittPoint.x = 1; ittPoint.x < width - 1; ittPoint.x++ ) {
      /* get 3x3 matrix centered at ittPoint.x, ittPoint.y */
      cv::Mat partMat = choshiImage->operator ()( cv::Rect( ittPoint.x-1, ittPoint.y-1, 3, 3 ) );
      choshiresource::ChoCluster *srcCluster = NULL;
      choshiresource::ChoCluster *dstCluster = NULL;
      int srcLabel = choshiImage->at<CVVEC_CHOSHI>( ittPoint.y, ittPoint.x )[choshiresource::CHANNEL_LABEL];

      /*-----------------------------------------------
       * compare labelNums between center and 8-neighbors,
       * add adjacency when labelnums is different.
       *-----------------------------------------------*/
      for( int i = 0; i < 3; i++ ) {
        for( int j = 0; j < 3; j++ ) {
          int dstLabel = partMat.at<CVVEC_CHOSHI>( i, j )[choshiresource::CHANNEL_LABEL];
          if( dstLabel == srcLabel )
            continue;
          srcCluster = m_param->getClusterAt( srcLabel );
          dstCluster = m_param->getClusterAt( dstLabel );
          srcCluster->addAdjacencyCluster( dstCluster );
          dstCluster->addAdjacencyCluster( srcCluster );
        }
      }
    }
  }
}

void ChoRAG::clearAdjacency() {
  int clusterSize = m_param->getClusterSize();
  for( int i = 0; i < clusterSize; i++ ) {
    choshiresource::ChoCluster *cluster = m_param->getClusterAt( i );
    if( !cluster )
      continue;
    cluster->clearAdjacencyClusterList();
  }
}

void ChoRAG::merge( MERGEWAY mergeway, cv::TermCriteria termCriTeria ) {
  std::list<cv::Vec2i> mergeLabelList;
  choshiresource::ChoCluster *srcCluster;
  choshiresource::ChoCluster *dstCluster;
  cv::Mat *choshiImage = m_param->getChoshiImage();

  int width = m_param->getImageWidth();
  int height = m_param->getImageHeight();

  for( int iter = 0; iter < termCriTeria.maxCount; iter++ ) {
    int clusterSize = m_param->getClusterSize();
    mergeLabelList.clear();
    /* check merge way and arrange adjacency */
    for( int clusterNum = 0; clusterNum < clusterSize; clusterNum++ ) {
      bool skipFlag = false;
      std::list<cv::Vec2i>::iterator itskip = mergeLabelList.begin();
      while( itskip != mergeLabelList.end() ) {
        cv::Vec2i skip = *itskip;
        if( clusterNum == skip.val[1] ) {
          skipFlag = true;
        }
        itskip++;
      }
      if( skipFlag )
        continue;
      srcCluster = m_param->getClusterAt( clusterNum );

      std::list<choshiresource::ChoCluster*>::iterator it = srcCluster->m_adjacencyClusterList.begin();
      while( it != srcCluster->m_adjacencyClusterList.end() ) {
        dstCluster = *it;

        cv::Vec2i mergeLabels; /* index0:merging, index1:merged, val-1:error */
        switch( mergeway ) {
        case ChoRAG::MARGE_BASED_COLOR: {
          mergeLabels = m_param->mergeBasedOnColor( srcCluster, dstCluster );
          break;
        }
        case ChoRAG::MARGE_BASED_PIXNUM: {
          mergeLabels = m_param->mergeBasedOnPixNum( srcCluster, dstCluster );
          break;
        }
        case ChoRAG::NUM:
          mergeLabels = cv::Vec2i( -1, -1 );
          break;
        }
        if( -1 != mergeLabels.val[0] ) {
          mergeLabelList.push_back( mergeLabels );
          it = srcCluster->m_adjacencyClusterList.begin();
          continue;
        }
        it++;
      }
    }
    /* merge cluster and merge choshiImage */
    std::list<cv::Vec2i>::iterator itmerge = mergeLabelList.begin();
    while( itmerge != mergeLabelList.end() ) {
      cv::Vec2i mergeLabel = *itmerge;
      if( -1 == mergeLabel.val[0] )
        continue;

      /* merge and assign merging label to merged label */
      cv::Point2i ittPoint;
      for( ittPoint.y = 0; ittPoint.y < height; ittPoint.y++ ) {
        for( ittPoint.x = 0; ittPoint.x < width; ittPoint.x++ ) {
          if( mergeLabel[1] == choshiImage->at<CVVEC_CHOSHI>( ittPoint.y, ittPoint.x )[choshiresource::CHANNEL_LABEL] )
            choshiImage->at<CVVEC_CHOSHI>( ittPoint.y, ittPoint.x )[choshiresource::CHANNEL_LABEL] = mergeLabel[0];
        }
      }
      m_param->deleteClusterAt( mergeLabel[1] );
      itmerge++;
    }

    /* reassign labelNum */
    m_param->reassignLabelNum();
  }
}

void ChoRAG::createDstImage() {
  cv::Mat *choshiImg = m_param->getChoshiImage();
  cv::Mat *dstImg = m_param->getDstImage();
  int width = m_param->getImageWidth();
  int height = m_param->getImageHeight();
  ImgProcColorMode eMode = m_param->getColorMode();
  cv::Vec3d color;
  cv::Vec3b rgb8;
  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      color.val[0] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[choshiresource::CHANNEL_R];
      color.val[1] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[choshiresource::CHANNEL_G];
      color.val[2] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[choshiresource::CHANNEL_B];

      /* select convert algorithm */
      switch( eMode ) {
      case COLORMODE_RGB: {
        color.val[0] = floor( color.val[0] );
        color.val[1] = floor( color.val[1] );
        color.val[2] = floor( color.val[2] );
        if( 0 > color.val[0] )
          color.val[0] = 0;
        if( 0 > color.val[1] )
          color.val[1] = 0;
        if( 0 > color.val[2] )
          color.val[2] = 0;
        if( 255 < color.val[0] )
          color.val[0] = 255;
        if( 255 < color.val[1] )
          color.val[1] = 255;
        if( 255 < color.val[2] )
          color.val[2] = 255;
        rgb8.val[2] = (uchar)color.val[0];
        rgb8.val[1] = (uchar)color.val[1];
        rgb8.val[0] = (uchar)color.val[2];
        break;
      }
      case COLORMODE_HVC: {
        cv::Vec3d rgbLinear = ImgProcUtil::calculateReverseMTM( color, NTSC_C );
        rgb8 = ImgProcUtil::convertIntegerRGB( rgbLinear );
        uchar temp;
        temp = rgb8.val[0];
        rgb8.val[0] = rgb8.val[2];
        rgb8.val[2] = temp;
        break;
      }
      case COLORMODE_LAB: {
        cv::Vec3d rgbLinear = ImgProcUtil::calculateReverseLAB( color, NTSC_C );
        rgb8 = ImgProcUtil::convertIntegerRGB( rgbLinear );
        uchar temp;
        temp = rgb8.val[0];
        rgb8.val[0] = rgb8.val[2];
        rgb8.val[2] = temp;
        break;
      }
      default:
        break;
      }
      dstImg->at<cv::Vec3b>( y, x ) = rgb8;
      // TODO move to cluster class
      choshiresource::ChoCluster *cluster = m_param->getClusterAt( choshiImg->at<cv::Vec6d>( y, x ).val[choshiresource::CHANNEL_LABEL] );
      cluster->m_meanRGB = rgb8;
    }
  }
}

ImgProcColorMode ChoRAG::convertColor( ConvertColorMode eMode ) {
  if( eMode >= CONVERTMODE_NUM ) {
    // TODO DebugMessage
    return COLORMODE_NUM;
  }

  ImgProcColorMode retVal = COLORMODE_NUM;

  cv::Mat *choshiImg = m_param->getChoshiImage();
  int width = m_param->getImageWidth();
  int height = m_param->getImageHeight();
  cv::Vec3d color;
  cv::Vec3d convertColor;
  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      color.val[0] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[0];
      color.val[1] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[1];
      color.val[2] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[2];

      /* select convert algorithm */
      switch( eMode ) {
      case RGB2LAB: {
        color = ImgProcUtil::convertLinearRGB( color );
        convertColor = ImgProcUtil::calculateLAB( color, NTSC_C );
        retVal = COLORMODE_LAB;
        break;
      }
      case RGB2LAB10: {
        color = ImgProcUtil::convertLinearRGB( color );
        convertColor = ImgProcUtil::calculateLAB10( color, NTSC_C );
        retVal = COLORMODE_LAB10;
        break;
      }
      case RGB2HVC: {
        color = ImgProcUtil::convertLinearRGB( color );
        convertColor = ImgProcUtil::calculateMTM( color, NTSC_C );
        retVal = COLORMODE_HVC;
        break;
      }
      case RGB2HVC10: {
        color = ImgProcUtil::convertLinearRGB( color );
        convertColor = ImgProcUtil::calculateMTM( color, NTSC_C );
        retVal = COLORMODE_HVC10;
        break;
      }
      case LAB2RGB: {
        color = ImgProcUtil::calculateReverseLAB( color, NTSC_C );
        convertColor = ImgProcUtil::convertIntegerRGB( color );
        retVal = COLORMODE_RGB;
        break;
      }
      case HVC2RGB: {
        color = ImgProcUtil::calculateReverseMTM( color, NTSC_C );
        convertColor = ImgProcUtil::convertIntegerRGB( color );
        retVal = COLORMODE_RGB;
        break;
      }
      default:
        break;
      } // TODO imprement reverse conversion
      choshiImg->at<CVVEC_CHOSHI>( y, x ).val[0] = convertColor.val[0];
      choshiImg->at<CVVEC_CHOSHI>( y, x ).val[1] = convertColor.val[1];
      choshiImg->at<CVVEC_CHOSHI>( y, x ).val[2] = convertColor.val[2];
    }
  }
  return retVal;
}


} /* namespace choshiimgproc */
