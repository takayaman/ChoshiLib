#include "choselectpapers.h"
#include "imgprocutil.h"
#include "basedef.h"

namespace choshiimgproc {

#define INITDIFFVALUE 10000000 /* give big value for init */

ChoSelectPapers::ChoSelectPapers() {
  m_param = NULL;
}

ChoSelectPapers::~ChoSelectPapers() {
  if( !m_param )
    this->clear();
}

bool ChoSelectPapers::init( BaseChoImgProcParam *param ) {
  /* Check class type */
  if( typeid( *param ) != typeid( ChoSelectPapersParam ) ) {
    // TODO DebugMessage
    return false;
  }
  m_param = pointer_cast<ChoSelectPapersParam*>( param );
  return true;
}

bool ChoSelectPapers::clear() {
  if( !m_param )
    delete m_param;
  return true;
}

bool ChoSelectPapers::execute() {
  if( !m_param ) {
    // TODO DebugMessage
    return false;
  }

  /* select papers */
  selectPapers();

  /* copy display Image */
  m_param->reflectClusterValToChoshiImage();
  createDstImage();

  return true;
}

void ChoSelectPapers::selectPapers() {
  ImgProcColorMode eMode = m_param->getColorMode();
  int paperSize = 0;
  int clusterSize = 0;
  double diffColor = INITDIFFVALUE;
  double tempDiff = 0.0;
  cv::Vec3d tempColor;
  cv::Vec3d tempHVC;
  cv::Vec3d tempLAB;
  cv::Vec3d tempRGB;
  std::string tempPaperName;
  choshiresource::ChoCluster *srcCluster = NULL;

  paperSize = m_param->getPaperSize();
  clusterSize = m_param->getClusterSize();

  for( int clIndex = 0; clIndex < clusterSize; clIndex++ ) {
    diffColor = INITDIFFVALUE;
    srcCluster = m_param->getClusterAt( clIndex );
    cv::Vec3d clusterColor;
    switch( eMode ) {
    case COLORMODE_HVC: {
      clusterColor = srcCluster->m_meanHVC;
      break;
    }
    case COLORMODE_LAB: {
      clusterColor = srcCluster->m_meanLAB;
      break;
    }
    case COLORMODE_RGB: {
      clusterColor = srcCluster->m_meanRGB;
      break;
    }
    default:
      // TODO DebugMessage:
      return;
    }
    for( int paperIndex = 0; paperIndex < paperSize; paperIndex++ ) {
      cv::Vec3d paperColor = m_param->getPaperColor( paperIndex );
      cv::Vec3d paperRGB = m_param->getPaperRGB( paperIndex );
      cv::Vec3d paperHVC = m_param->getPaperHVC( paperIndex );
      cv::Vec3d paperLAB = m_param->getPaperLAB( paperIndex );
      std::string paperName = m_param->getPaperName( paperIndex );
      tempDiff = ImgProcUtil::calculateDiff( clusterColor, paperColor, m_param->getColorMode() );
      if( tempDiff <= diffColor ) {
        diffColor = tempDiff;
        tempColor = paperColor;
        tempRGB = paperRGB;
        tempHVC = paperHVC;
        tempLAB = paperLAB;
        tempPaperName = paperName;
      }
    }
    srcCluster->m_meanHVC = tempHVC;
    srcCluster->m_meanLAB = tempLAB;
    srcCluster->m_meanRGB = tempRGB;
    srcCluster->m_paperName = tempPaperName;
  }
}

void ChoSelectPapers::createDstImage() {
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

ImgProcColorMode ChoSelectPapers::convertColor( ConvertColorMode eMode ) {
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
