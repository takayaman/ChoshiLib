#include "chomeanshift.h"
#include "imgprocutil.h"
#include <math.h>

namespace choshiimgproc {

ChoMeanShift::ChoMeanShift() {
  m_param = NULL;
}

ChoMeanShift::~ChoMeanShift() {
  if( !m_param )
    this->clear();
}

bool ChoMeanShift::init( BaseChoImgProcParam *param ) {
  /* Check class type */
  if( typeid( *param ) != typeid( ChoMeanShiftParam )) {
    // TODO DebugMessage
    return false;
  }
  m_param = pointer_cast<ChoMeanShiftParam *>( param );
  return true;
}

bool ChoMeanShift::clear() {
  if( !m_param )
    delete m_param;
  return true;
}

bool ChoMeanShift::execute() {
  if( !m_param ) {
    // TODO DebugMessage
    return false;
  }

  /* execute meanshift */
  bool retval = meanshift();

  /* copy display image */
  createDstImage();

  return retval;
}

bool ChoMeanShift::meanshift() {
  cv::TermCriteria termCriteria = m_param->getCvTermCriteria();
  int width = m_param->getImageWidth();
  int height = m_param->getImageHeight();
  cv::Mat *choshiImg = m_param->getChoshiImage();

  /*-----------------------------------------------
   * Initialize
   *-----------------------------------------------*/

  cv::Mat maskImg( height, width, CV_8UC1 );

  cv::Point2i ittPoint; /* pixel of interest */

  /*-----------------------------------------------
   * meanshift procedure
   *-----------------------------------------------*/
  for( ittPoint.y = 0; ittPoint.y < height; ittPoint.y++ ) {
    for( ittPoint.x = 0; ittPoint.x < width; ittPoint.x++ ) {

      if( 255 == maskImg.at<unsigned char>( ittPoint.y, ittPoint.x ) )
        continue;

      /* For accelation.
       * passedPoint should converge on same point at final iteration.
       * so, operation at passedPoint is ignored.
       */
      cv::Point2i passedPoint[termCriteria.maxCount];
      //cv::Point2i passedPoint[5];
      for( int i = 0; i < termCriteria.maxCount; i++ )
        passedPoint[i].x = -1, passedPoint[i].y = -1; /* initialize */
      CVVEC_CHOSHI writeData = actionMeanShift( ittPoint, termCriteria, passedPoint );
      choshiImg->at<CVVEC_CHOSHI>( ittPoint.y, ittPoint.x ) = writeData;
      /* write same data to passed point */
      for( int i = 0; i < termCriteria.maxCount; i++ ) {
        if( passedPoint[i].x < 0 || passedPoint[i].x >= width || passedPoint[i].y < 0 || passedPoint[i].y >= height )
          break;
        choshiImg->at<CVVEC_CHOSHI>( passedPoint[i].y, passedPoint[i].x ) = writeData;
        maskImg.at<unsigned char>( passedPoint[i].y, passedPoint[i].x ) = 255;
      }
    }
  }

  return true;

}

CVVEC_CHOSHI ChoMeanShift::actionMeanShift( cv::Point2i ittPt, cv::TermCriteria termCrit  ,cv::Point2i *passedPoint ) {
  CVVEC_CHOSHI retVal;
  retVal.zeros();
  float sRadius = m_param->getSpatialRadius();
  float cRadius = m_param->getColorRadius();

  cv::Vec3d srcColor;
  srcColor.zeros();
  cv::Mat *choshiImg = m_param->getChoshiImage();
  int innerPixCount = 0; /* pixels which is inside of color radius */
  cv::Vec3d totalColorDiff = 0;
  double totalDiffX = 0;
  double totalDiffY = 0;

  /* Get Color at interrest point */
  for( int nChl = 0; nChl < CHANNELS_COLOR; nChl++ )
    srcColor.val[nChl] = choshiImg->at<CVVEC_CHOSHI>( ittPt.y, ittPt.x ).val[nChl];
  int meanX = ittPt.x;
  int meanY = ittPt.y;
  /* mean shift */
  for( int iter = 0; iter < termCrit.maxCount; iter++ ) {
    int minX, minY, maxX, maxY;
    int windowY, windowX;
    double diffColor = 0;
    bool stopFlag = false;
    /* define window size */
    minX = cvRound( (double)meanX - sRadius );
    minX = MAX( minX, 0 );
    minY = cvRound( (double)meanY - sRadius );
    minY = MAX( minY, 0 );
    maxX = cvRound( (double)meanX + sRadius );
    maxX = MIN( maxX, m_param->getImageWidth() - 1 );
    maxY = cvRound( (double)meanY + sRadius );
    maxY = MIN( maxY, m_param->getImageHeight() - 1 );

    /* window search */
    for( windowY = minY; windowY <= maxY; windowY++ ) {
      int innerRowCount = 0;
      for( windowX = minX; windowX < maxX; windowX++ ) {
        cv::Vec3d windowColor;
        for( int nChl = 0; nChl < CHANNELS_COLOR; nChl++ ) {
          windowColor.val[nChl] = choshiImg->at<CVVEC_CHOSHI>( windowY, windowX ).val[nChl];
        }
        diffColor = ImgProcUtil::calculateDiff( srcColor, windowColor, m_param->getColorMode() );
        if( diffColor <= cRadius ) {
          totalColorDiff += windowColor;
          totalDiffX += windowX;
          innerRowCount++;
        }
      }
      innerPixCount += innerRowCount;
      totalDiffY += windowY * innerRowCount;
    }
    /* isolated color pixel */
    if( 0 == innerPixCount ) {
      for( int nChl = 0; nChl < CHANNELS_COLOR; nChl++ )
        retVal.val[nChl] = srcColor.val[nChl];
      retVal.val[choshiresource::CHANNEL_X] = ittPt.x;
      retVal.val[choshiresource::CHANNEL_Y] = ittPt.y;
      retVal.val[choshiresource::CHANNEL_LABEL] = -1;
      return retVal;
    }
    /* calc difference */
    double reiprocalPixCount = 1./innerPixCount;
    for( int nChl = 0; nChl < CHANNELS_COLOR; nChl++ ) {
      double diff = totalColorDiff.val[nChl] * reiprocalPixCount;
      retVal.val[nChl] = diff;
    }
    retVal.val[choshiresource::CHANNEL_X] = (double)cvRound(totalDiffX * reiprocalPixCount);
    retVal.val[choshiresource::CHANNEL_Y] = (double)cvRound(totalDiffY * reiprocalPixCount);
    retVal.val[choshiresource::CHANNEL_LABEL] = -1;

    if( (retVal.val[choshiresource::CHANNEL_X] - meanX < 1.0)
        && (retVal.val[choshiresource::CHANNEL_Y] - meanY < 1.0)
        && diffColor <= termCrit.epsilon) {
      stopFlag = true;
    }
    /* copy value for next iteration */
    for( int nChl = 0; nChl < CHANNELS_COLOR; nChl++ )
      srcColor.val[nChl] = retVal.val[nChl];
    meanX = (int)retVal.val[choshiresource::CHANNEL_X];
    meanY = (int)retVal.val[choshiresource::CHANNEL_Y];

    /* check passed point */
    passedPoint->x = meanX;
    passedPoint->y = meanY;
    passedPoint++;

    /* For Accelation */
    if( stopFlag )
      break;
  }
  return retVal;
}

void ChoMeanShift::createDstImage() {
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

ImgProcColorMode ChoMeanShift::convertColor( ConvertColorMode eMode ) {
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
