#include "cholabeling.h"
#include "imgprocutil.h"

namespace choshiimgproc {

ChoLabeling::ChoLabeling() {
  m_param = NULL;
  m_tempLabelCount = 0;
}

ChoLabeling::~ChoLabeling() {
  if( !m_param )
    this->clear();
}

bool ChoLabeling::init( BaseChoImgProcParam *param ) {
  /* Check class type */
  if( typeid( *param ) != typeid( ChoLabelingParam ) )
    return false;
  m_param = pointer_cast<ChoLabelingParam*>( param );
  return true;
}

bool ChoLabeling::clear() {
  if( !m_param )
    delete m_param;
  return true;
}

bool ChoLabeling::execute() {
  if( !m_param )
    return false;

  bool retVal = actionLabeling();

  /* Copy Display Image */
  m_param->reflectClusterValToChoshiImage();
  createDstImage();

  return retVal;
}

bool ChoLabeling::actionLabeling() {
  cv::Mat *choshiImage = m_param->getChoshiImage();
  int width = m_param->getImageWidth();
  int height = m_param->getImageHeight();
  int labelNum = 0;

  /*-----------------------------------------------
   * Initialize
   *-----------------------------------------------*/
  cv::Point2i ittPoint; /* pixel of interest */
  CVVEC_CHOSHI meanVal;
  meanVal.zeros();

  /*-----------------------------------------------
   * labeling procedure
   *-----------------------------------------------*/
  for( ittPoint.y = 0; ittPoint.y < height; ittPoint.y++ ) {
    for( ittPoint.x = 0; ittPoint.x < width; ittPoint.x++ ) {
      meanVal = choshiImage->at<CVVEC_CHOSHI>( ittPoint.y, ittPoint.x );

      /* check already assigned */
      if( meanVal.val[choshiresource::CHANNEL_LABEL] >= 0 )
        continue;

      /*--- assign new labels ---------------------*/
      m_tempLabelCount = 0;
      meanVal = fill( ittPoint, labelNum );
      if( m_tempLabelCount == 0 )
        continue;

      /*--- cleate new Cluster --------------------*/
      choshiresource::ChoCluster *cluster = new choshiresource::ChoCluster();
      cluster->m_meanXY.val[0] = meanVal[choshiresource::CHANNEL_X];
      cluster->m_meanXY.val[1] = meanVal[choshiresource::CHANNEL_Y];
      ImgProcColorMode eMode = m_param->getColorMode();
      switch(eMode) {
      case COLORMODE_RGB: {
        cluster->m_meanRGB[0] = (uchar)meanVal[choshiresource::CHANNEL_R];
        cluster->m_meanRGB[1] = (uchar)meanVal[choshiresource::CHANNEL_G];
        cluster->m_meanRGB[2] = (uchar)meanVal[choshiresource::CHANNEL_B];
        cv::Vec3b rgb8 = cv::Vec3d( (uchar)meanVal[choshiresource::CHANNEL_R], (uchar)meanVal[choshiresource::CHANNEL_G], (uchar)meanVal[choshiresource::CHANNEL_B] );
        cv::Vec3d rgbLinear = ImgProcUtil::convertLinearRGB( rgb8 );
        cluster->m_meanHVC = ImgProcUtil::calculateMTM( rgbLinear, NTSC_C );
        cluster->m_meanLAB = ImgProcUtil::calculateLAB( rgbLinear, NTSC_C );
        break;
      }
      case COLORMODE_LAB: {
        cluster->m_meanLAB[0] = meanVal[choshiresource::CHANNEL_R];
        cluster->m_meanLAB[1] = meanVal[choshiresource::CHANNEL_G];
        cluster->m_meanLAB[2] = meanVal[choshiresource::CHANNEL_B];
        cv::Vec3d rgbLinear = ImgProcUtil::calculateReverseLAB( cluster->m_meanLAB, NTSC_C );
        cv::Vec3b rgb8 = ImgProcUtil::convertIntegerRGB( rgbLinear );
        cluster->m_meanRGB[0] = rgb8.val[0];
        cluster->m_meanRGB[1] = rgb8.val[1];
        cluster->m_meanRGB[2] = rgb8.val[2];
        cv::Vec3b hvc = ImgProcUtil::calculateMTM( rgbLinear, NTSC_C );
        cluster->m_meanHVC[0] = hvc.val[0];
        cluster->m_meanHVC[1] = hvc.val[1];
        cluster->m_meanHVC[2] = hvc.val[2];
        break;
      }
      case COLORMODE_HVC: {
        cluster->m_meanHVC[0] = meanVal[choshiresource::CHANNEL_R];
        cluster->m_meanHVC[1] = meanVal[choshiresource::CHANNEL_G];
        cluster->m_meanHVC[2] = meanVal[choshiresource::CHANNEL_B];
        cv::Vec3d rgbLinear = ImgProcUtil::calculateReverseMTM( cluster->m_meanHVC, NTSC_C );
        cv::Vec3b rgb8 = ImgProcUtil::convertIntegerRGB( rgbLinear );
        cluster->m_meanRGB[0] = rgb8.val[0];
        cluster->m_meanRGB[1] = rgb8.val[1];
        cluster->m_meanRGB[2] = rgb8.val[2];
        cv::Vec3d lab = ImgProcUtil::calculateLAB( rgbLinear, NTSC_C );
        cluster->m_meanLAB[0] = lab.val[0];
        cluster->m_meanLAB[1] = lab.val[1];
        cluster->m_meanLAB[2] = lab.val[2];
        break;
      }
      default:
        break;
      }
      cluster->m_labelNum = labelNum;
      cluster->m_holdPixelNum = m_tempLabelCount;
      m_param->addCluster( cluster );
      labelNum++;
    }
  }
  return true;
}

cv::Vec6d ChoLabeling::fill( cv::Point2i ittPoint, int labelNum ) {
  cv::Mat *choshiImage = m_param->getChoshiImage();
  CVVEC_CHOSHI centerVal;
  float cRadius = m_param->getColorRadius();
  centerVal.zeros();
  std::list<cv::Point2i> neighbors;
  //cv::Point2i neighborPos[8];
  std::vector<cv::Point2i> neighborsPos;
  cv::Point2i centerPos = ittPoint;
  cv::Vec6d totalVal;
  totalVal.zeros();

  do {

    if( !neighbors.empty() ) {
      cv::Point2i temp = neighbors.front();
      centerPos.x = temp.x;
      centerPos.y = temp.y;
      neighbors.pop_front();
    }

    if( FOUR_NEIGHBORS == m_param->getNeighborsMode() ) {
      neighborsPos.push_back( cv::Point2i( centerPos.x, centerPos.y - 1 ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x - 1, centerPos.y ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x + 1, centerPos.y ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x, centerPos.y + 1 ) );
    } else if( EIGHT_NEIGHBORS == m_param->getNeighborsMode() ) {
      neighborsPos.push_back( cv::Point2i( centerPos.x - 1, centerPos.y - 1 ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x, centerPos.y - 1 ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x + 1, centerPos.y - 1 ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x - 1, centerPos.y ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x + 1, centerPos.y ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x - 1, centerPos.y + 1 ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x, centerPos.y + 1 ) );
      neighborsPos.push_back( cv::Point2i( centerPos.x + 1, centerPos.y + 1 ) );
    }

    /*
            neighborPos[0].y = centerPos.y - 1; neighborPos[0].x = centerPos.x - 1;
            neighborPos[1].y = centerPos.y - 1; neighborPos[1].x = centerPos.x;
            neighborPos[2].y = centerPos.y - 1; neighborPos[2].x = centerPos.x + 1;
            neighborPos[3].y = centerPos.y; neighborPos[3].x = centerPos.x - 1;
            neighborPos[4].y = centerPos.y; neighborPos[4].x = centerPos.x + 1;
            neighborPos[5].y = centerPos.y + 1; neighborPos[5].x = centerPos.x - 1;
            neighborPos[6].y = centerPos.y + 1; neighborPos[6].x = centerPos.x;
            neighborPos[7].y = centerPos.y + 1; neighborPos[7].x = centerPos.x + 1;
    */

    /* assign label and write back */
    centerVal = choshiImage->at<CVVEC_CHOSHI>( centerPos.y, centerPos.x );
    if( centerVal.val[choshiresource::CHANNEL_LABEL] < 0 ) {
      centerVal.val[choshiresource::CHANNEL_LABEL] = labelNum;
      choshiImage->at<CVVEC_CHOSHI>( centerPos.y, centerPos.x ) = centerVal;
      m_tempLabelCount++;
      totalVal += centerVal;
    }

    cv::Vec3d centerColor;
    centerColor.zeros();
    for( int nChl = 0; nChl < CHANNELS_COLOR; nChl++ )
      centerColor.val[nChl] = centerVal.val[nChl];

    /* check and assign neighbors */
    for( uint i = 0; i < neighborsPos.size() /*8*/; i++ ) {
      /* check point is inner image border */
      if( neighborsPos[i].y >= 0 && neighborsPos[i].y <= m_param->getImageHeight() - 1
          && neighborsPos[i].x >= 0 && neighborsPos[i].x <= m_param->getImageWidth() - 1 )
        //if( neighborPos[i].y >= 0 && neighborPos[i].y <= m_param->getImageHeight() - 1
        //          && neighborPos[i].x >= 0 && neighborPos[i].x <= m_param->getImageWidth() - 1 )
      {
        CVVEC_CHOSHI neighborVal = choshiImage->at<CVVEC_CHOSHI>( neighborsPos[i].y, neighborsPos[i].x );
        //CVVEC_CHOSHI neighborVal = choshiImage->at<CVVEC_CHOSHI>( neighborPos[i].y, neighborPos[i].x );
        /* check already labeled */
        if( neighborVal.val[choshiresource::CHANNEL_LABEL] >= 0 )
          continue;
        cv::Vec3d neighborColor;
        for( int nChl = 0; nChl < CHANNELS_COLOR; nChl++ )
          neighborColor.val[nChl] = neighborVal.val[nChl];
        /* assign label and write back */
        if( cRadius > ImgProcUtil::calculateDiff( centerColor, neighborColor, m_param->getColorMode() ) ) {
          neighborVal.val[choshiresource::CHANNEL_LABEL] = labelNum;
          choshiImage->at<CVVEC_CHOSHI>( neighborsPos[i].y, neighborsPos[i].x ) = neighborVal;
          //choshiImage->at<CVVEC_CHOSHI>( neighborPos[i].y, neighborPos[i].x ) = neighborVal;
          //neighbors.push_back( neighborPos[i] );
          //cv::Point2i tempPos;
          //tempPos.x = neighborsPos[i].x;
          //tempPos.y = neighborsPos[i].y;
          neighbors.push_back( neighborsPos[i] );
          m_tempLabelCount++;
          totalVal += neighborVal;
        }
      }
    }
    neighborsPos.clear();
  } while( !neighbors.empty() );

  neighbors.clear();

  totalVal /= m_tempLabelCount;

  return totalVal;
}

void ChoLabeling::createDstImage() {
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
    }
  }
}

ImgProcColorMode ChoLabeling::convertColor( ConvertColorMode eMode ) {
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
