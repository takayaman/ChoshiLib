/*=============================================================================
 * Project : ChoImgProc
 * Code : imgprocutil.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Utility functions.
 *===========================================================================*/

#ifndef IMGPROCUTIL_H
#define IMGPROCUTIL_H

#include <opencv2/opencv.hpp>
#include "./basedef.h"

namespace choshiimgproc {

/** Definition of color space and white point.
 */
typedef enum DEFCOLORANDWHITE_TAG {
  SRGB_D65 = 0,                         /**< Color:SRGB, White:D65 */
  ADOBE_D65,                            /**< Color:Adobe, White:D65 */
  NTSC_C                                /**< Color:NTSC, White:C-Light */
} DefColorAndWhite;

/** Utility functions.
 */
class ImgProcUtil {
 public:
  /** Default constructor.
   */
  ImgProcUtil(void);

 public:
  /*--- Static Methods -------------------------------------------------------*/
  /** RGB8 -> LinearRGB.
   * @param rgb8 RGB8(RGB color in 3 bytes).
   * @return Linear RGB color.
   */
  static cv::Vec3d convertLinearRGB(cv::Vec3b rgb8);

  /** LinearRGB -> RGB8
   * @param rgbLinear LinearRGB
   * @return RGB8
   */
  static cv::Vec3b convertIntegerRGB(cv::Vec3d rgbLinear);

  /* calculate XYZ */
  /** 
   * @param rgb LinearRGB
   * @param eMode Definition of color and white point.
   * @return XYZ
   */
  static cv::Vec3d calculateXYZ(cv::Vec3d rgb, DefColorAndWhite eMode);

  /** 
   * @param rgb LinearRGB
   * @param eMode Definition of color and white point.
   * @return XYZ10
   */
  static cv::Vec3d calculateXYZ10(cv::Vec3d rgb, DefColorAndWhite eMode);
  /* calculate MTM */
  /** 
   * @param rgb LinearRGB
   * @param eMode Definition of color and white point.
   * @return MTM(HVC)
   */
  static cv::Vec3d calculateMTM(cv::Vec3d rgb, DefColorAndWhite eMode);
  /** 
   * @param hvc HVC(MTM)
   * @param eMode Definition of color and white point.
   * @return XYZ
   */
  static cv::Vec3d calculateReverseMTM(cv::Vec3d hvc, DefColorAndWhite eMode);
  /* calculateLAB */
  /** 
   * @param rgb LinearRGB
   * @param eMode Definition of color and white point.
   * @return LAB
   */
  static cv::Vec3d calculateLAB(cv::Vec3d rgb, DefColorAndWhite eMode);
  /** 
   * @param rgb LinearRGB
   * @param eMode Definition of color and white point.
   * @return LAB10
   */
  static cv::Vec3d calculateLAB10(cv::Vec3d rgb, DefColorAndWhite eMode);
  /** 
   * @param lab LAB
   * @param eMode Definition of color and white point.
   * @return XYZ
   */
  static cv::Vec3d calculateReverseLAB(cv::Vec3d lab, DefColorAndWhite eMode);

  /* calculate Diff of color */
  /** 
   * @param color0 
   * @param color1 
   * @param eMode Color mode
   * @return Difference between color0 and color1.
   */
  static double calculateDiff(cv::Vec3d color0, cv::Vec3d color1, ImgProcColorMode eMode);

  /** 
   * @param srcChoshiImg 
   * @param dstMaskImg 
   * @param labelNum 
   */
  static void writeMask(cv::Mat *srcChoshiImg, cv::Mat *dstMaskImg, int labelNum);

  /** 
   * @param srcMaskImg 
   * @param dstContourImg 
   */
  static void writeContour(cv::Mat *srcMaskImg, cv::Mat *dstContourImg);

 private:
  /* calclutate XYZ elements */
  /** 
   * @param color 
   * @param eMode Definition of color and white point.
   * @return X of XYZ
   */
  static double calcX(cv::Vec3d color, DefColorAndWhite eMode);
  /** 
   * @param color 
   * @param eMode Definition of color and white point.
   * @return Y of XYZ
   */
  static double calcY(cv::Vec3d color, DefColorAndWhite eMode);
  /** 
   * @param color 
   * @param eMode Definition of color and white point.
   * @return Z of XYZ
   */
  static double calcZ(cv::Vec3d color, DefColorAndWhite eMode);

  /** 
   * @param color 
   * @param eMode Definition of color and white point.
   * @return X of XYZ10
   */
  static double calcX10(cv::Vec3d color, DefColorAndWhite eMode);

  /** 
   * @param color 
   * @param eMode Definition of color and white point.
   * @return Y of XYZ10
   */
  static double calcY10(cv::Vec3d color, DefColorAndWhite eMode);

  /** 
   * @param color 
   * @param eMode Definition of color and white point.
   * @return Z of XYZ10
   */
  static double calcZ10(cv::Vec3d color, DefColorAndWhite eMode);

  /* calculate MTM elements */
  /** 
   * @param a 
   * @return 
   */
  static double calcV(double a);
  /** 
   * @param xyzColor 
   * @return 
   */
  static double calcM1(cv::Vec3d xyzColor);

  /** 
   * @param xyzColor 
   * @return 
   */
  static double calcM2(cv::Vec3d xyzColor);

  /** 
   * @param xyzColor 
   * @return 
   */
  static double calcM3(cv::Vec3d xyzColor);

  /** 
   * @param xyzColor 
   * @return 
   */
  static double calcAdamsH(cv::Vec3d xyzColor);

  /** 
   * @param xyzColor 
   * @return 
   */
  static double calcAdamsV(cv::Vec3d xyzColor);
  /** 
   * @param xyzColor 
   * @return 
   */
  static double calcAdamsC(cv::Vec3d xyzColor);

  /* calculate LAB elements */
  /** 
   * @param color 
   * @return L of LAB
   */
  static double calcLABL(cv::Vec3d color);
  /** 
   * @param color 
   * @return A of LAB
   */
  static double calcLABA(cv::Vec3d color);
  /** 
   * @param color 
   * @return B of LAB
   */
  static double calcLABB(cv::Vec3d color);
  /** 
   * @param input 
   * @return 
   */
  static double calcFunc(double input);
  /** 
   * @param color 
   * @return L of LAB10
   */
  static double calcLAB10L(cv::Vec3d color);
  /** 
   * @param color 
   * @return A of LAB10
   */
  static double calcLAB10A(cv::Vec3d color);
  /** 
   * @param color 
   * @return B of LAB10
   */
  static double calcLAB10B(cv::Vec3d color);

  /* calculate Difference */
  /** 
   * @param rgb0 
   * @param rgb1 
   * @return Differance between rgb0 and rgb1.
   */
  static double calcDiffRGB(cv::Vec3d rgb0, cv::Vec3d rgb1);
  /** 
   * @param lab0 
   * @param lab1 
   * @return Difference between lab0 and lab1.
   */
  static double calcDiffLAB(cv::Vec3d lab0, cv::Vec3d lab1);
  /** 
   * @param hvc0 
   * @param hvc1 
   * @return Difference between hvc0 and hvc1.
   */
  static double calcDiffHVC(cv::Vec3d hvc0, cv::Vec3d hvc1);
};

} /* namespace choshiimgproc */

#endif  // IMGPROCUTIL_H
