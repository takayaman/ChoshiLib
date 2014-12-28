#ifndef IMGPROCUTIL_H
#define IMGPROCUTIL_H

#include <opencv2/opencv.hpp>
#include "basedef.h"

namespace choshiimgproc {

typedef enum DEFCOLORANDWHITE_TAG {
  SRGB_D65 = 0,
  ADOBE_D65,
  NTSC_C
} DefColorAndWhite;

class ImgProcUtil {
 public:
  ImgProcUtil();

 public:
  /* Static Methods */
  /* convert RGB8 from/to linearRGB */
  static cv::Vec3d convertLinearRGB( cv::Vec3b rgb8 );
  static cv::Vec3b convertIntegerRGB( cv::Vec3d rgbLinear );

  /* calculate XYZ */
  static cv::Vec3d calculateXYZ( cv::Vec3d rgb, DefColorAndWhite eMode );
  static cv::Vec3d calculateXYZ10( cv::Vec3d rgb, DefColorAndWhite eMode );
  /* calculate MTM */
  static cv::Vec3d calculateMTM( cv::Vec3d rgb, DefColorAndWhite eMode );
  static cv::Vec3d calculateReverseMTM( cv::Vec3d hvc, DefColorAndWhite eMode );
  /* calculateLAB */
  static cv::Vec3d calculateLAB( cv::Vec3d rgb, DefColorAndWhite eMode );
  static cv::Vec3d calculateLAB10( cv::Vec3d rgb, DefColorAndWhite eMode );
  static cv::Vec3d calculateReverseLAB( cv::Vec3d lab, DefColorAndWhite eMode );

  /* calculate Diff of color */
  static double calculateDiff( cv::Vec3d color0, cv::Vec3d color1, ImgProcColorMode eMode );

  static void writeMask( cv::Mat *srcChoshiImg, cv::Mat *dstMaskImg, int labelNum );
  static void writeContour( cv::Mat *srcMaskImg, cv::Mat *dstContourImg );

 private:
  /* calclutate XYZ elements */
  static double calcX( cv::Vec3d color, DefColorAndWhite eMode );
  static double calcY( cv::Vec3d color, DefColorAndWhite eMode );
  static double calcZ( cv::Vec3d color, DefColorAndWhite eMode );
  static double calcX10( cv::Vec3d color, DefColorAndWhite eMode );
  static double calcY10( cv::Vec3d color, DefColorAndWhite eMode );
  static double calcZ10( cv::Vec3d color, DefColorAndWhite eMode );
  /* calculate MTM elements */
  static double calcV( double a );
  static double calcM1( cv::Vec3d xyzColor );
  static double calcM2( cv::Vec3d xyzColor );
  static double calcM3( cv::Vec3d xyzColor );
  static double calcAdamsH( cv::Vec3d xyzColor );
  static double calcAdamsV( cv::Vec3d xyzColor );
  static double calcAdamsC( cv::Vec3d xyzColor );
  /* calculate LAB elements */
  static double calcLABL( cv::Vec3d color );
  static double calcLABA( cv::Vec3d color );
  static double calcLABB( cv::Vec3d color );
  static double calcFunc( double input );
  static double calcLAB10L( cv::Vec3d color );
  static double calcLAB10A( cv::Vec3d color );
  static double calcLAB10B( cv::Vec3d color );

  /* calculate Difference */
  static double calcDiffRGB( cv::Vec3d rgb0, cv::Vec3d rgb1 );
  static double calcDiffLAB( cv::Vec3d lab0, cv::Vec3d lab1 );
  static double calcDiffHVC( cv::Vec3d hvc0, cv::Vec3d hvc1 );
};

} /* namespace choshiimgproc */

#endif // IMGPROCUTIL_H
