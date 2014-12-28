/*=============================================================================
 * Project : ChoImgProc
 * Code : basechoimgprocparam.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class is the base class of image processing parameters.
 *===========================================================================*/

#ifndef BASECHOPROCPARAM_H
#define BASECHOPROCPARAM_H

#include <opencv2/opencv.hpp>

#include "./choimgproc_global.h"
#include "./choresource.h"
#include "./basedef.h"

namespace choshiimgproc {

/** This class is tha base class of image processing parameters.
 */
class CHOIMGPROCSHARED_EXPORT BaseChoImgProcParam {
 public:
  /** Constructor
   * @param srcImg Source image.
   * @param dstImg Destination image.
   * @return None.
   */
  BaseChoImgProcParam(const cv::Mat *srcImg, cv::Mat *dstImg);

  /** Destructor
   * @return None
   */
  virtual ~BaseChoImgProcParam(void);

  /*--- Accessor -------------------------------------------------------------*/

  /** 
   * @return Image width.
   */
  int getImageWidth(void);

  /** 
   * @return Image height.
   */
  int getImageHeight(void);

  /** 
   * @return Image channels.
   */
  int getImageChannels(void);

  /** 
   * @return Image step.
   */
  int getImageStep(void);

  /** 
   * @return Image elsement size.
   */
  int getImageElementSize(void);

  /** 
   * @param eMode Color mode.
   */
  void setColorMode(ImgProcColorMode eMode);

  /** 
   * @return Color mode.
   */
  ImgProcColorMode getColorMode(void);

  /** 
   * @return Pointer of source image.
   */
  cv::Mat *getSrcImage(void);

  /** 
   * @return Pointer of destination image.
   */
  cv::Mat *getDstImage(void);

  /** 
   * @return Pointer of Choshi image.
   */
  cv::Mat *getChoshiImage(void);

  /** 
   * @param resource Pointer of resource.
   */
  void setResource(choshiresource::ChoResource *resource);

  /** 
   * @return Pointer of resource.
   */
  choshiresource::ChoResource *getResource(void);

  /** 
   * @return true : success, false : fail.
   */
  bool reflectClusterValToChoshiImage(void);

 protected:
  cv::Mat *m_srcImage;                  /**< Pointer of source image. */
  cv::Mat *m_dstImage;                  /**< Pointer of destination image. */
  choshiresource::ChoResource *m_resource; /**< Pointer of resource. */
  ImgProcColorMode m_colorMode;         /**< Color mode */


}; /* class BaseChoImgProcParam */

} /* namespace choshiimgproc */

#endif // BASECHOPROCPARAM_H
