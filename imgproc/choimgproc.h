/*=============================================================================
 * Project : ChoImgProc
 * Code : choimgproc.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class is the base class of image processing executer.
 *===========================================================================*/

#ifndef CHOIMGPROC_H
#define CHOIMGPROC_H

#include <opencv2/opencv.hpp>
#include <string>

#include "./choimgproc_global.h"
#include "./ichoimgproc.h"
#include "./basechoimgprocparam.h"

namespace choshiimgproc {

/** This class the base class of image processing executer.
 */
class CHOIMGPROCSHARED_EXPORT ChoImgProc {
 public:
  /** Default Constructor
   * @return None
   */
  ChoImgProc(void);

  /** 
   * @param filename File name to load image.
   * @return None
   */
  explicit ChoImgProc(const std::string &filename);

  /** Destructor
   * @return None
   */
  ~ChoImgProc(void);

  /*--- Accessor -------------------------------------------------------------*/

  /** 
   * @param filename File name to load image.
   * @return true : success, false : fail.
   */
  bool setImage(const std::string &filename);

  /** 
   * @param image Pointer of image.
   * @return true : success, false : fail.
   */
  bool setImage(cv::Mat *image);

  /** 
   * @return Pointer of input image.
   */
  cv::Mat* getInputImage(void);

  /** 
   * @return Pointer of display image.
   */
  cv::Mat* getDispImage(void);

  /** 
   * @param proc Image processing algorithms.
   */
  void setImgProc(const IChoImgProc *proc);

  /*--- Methods --------------------------------------------------------------*/

  /** 
   * @param param Pointer of parameters of algorithms.
   * @return true : success, false : fail.
   */
  bool init(BaseChoImgProcParam *param);

  /** 
   * @return true : success, false : fail.
   */
  bool clear(void);

  /** 
   * @return true : success, false : fail.
   */
  bool execute(void);

  /** 
   * @param eMode Color mode to convert.
   * @return Color mode of converted.
   */
  ImgProcColorMode convertColor(ConvertColorMode eMode);

  /** Create destination image.
   */
  void createDstImage(void);

 private:
  cv::Mat m_inputImage;                 /**< Input image. */
  cv::Mat m_dispImage;                  /**< Display image. */
  IChoImgProc *m_proc;                  /**< Image Processing algorithm */
};  /* class ChoImgProc */

}   /* namespace choshiimgproc */

#endif  // CHOIMGPROC_H
