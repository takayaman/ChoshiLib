/*=============================================================================
 * Project : ChoshiImgProc
 * Code : basechoimgprocparam.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Base class of image processing parameters
 *===========================================================================*/

#ifndef BASECHOPROCPARAM_H
#define BASECHOPROCPARAM_H

#include <opencv2/opencv.hpp>

#include "./choimgproc_global.h"
#include "./choresource.h"
#include "./basedef.h"

namespace choshiimgproc {

class CHOIMGPROCSHARED_EXPORT BaseChoImgProcParam {
 public:
  /* Constructor */
  BaseChoImgProcParam(const cv::Mat *srcImg, cv::Mat *dstImg);
  /* Destructor */
  virtual ~BaseChoImgProcParam(void);

  /* Accessor */
  int getImageWidth(void);
  int getImageHeight(void);
  int getImageChannels(void);
  int getImageStep(void);
  int getImageElementSize(void);
  void setColorMode(ImgProcColorMode eMode);
  ImgProcColorMode getColorMode(void);
  cv::Mat *getSrcImage(void);
  cv::Mat *getDstImage(void);
  cv::Mat *getChoshiImage(void);
  void setResource(choshiresource::ChoResource *resource);
  choshiresource::ChoResource *getResource(void);
  bool reflectClusterValToChoshiImage(void);

 protected:
  cv::Mat *m_srcImage;
  cv::Mat *m_dstImage;
  choshiresource::ChoResource *m_resource;
  ImgProcColorMode m_colorMode;
}; /* class BaseChoImgProcParam */

} /* namespace choshiimgproc */

#endif // BASECHOPROCPARAM_H
