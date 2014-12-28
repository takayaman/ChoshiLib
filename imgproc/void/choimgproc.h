/******************************************************************************
 * Project      : ChoImgProc
 * File         : choimgproc.h
 * Copyright    : 2013 (c) Natsuki Takayama, UEC. Tokyo
 * The Header file of ChoImgProc class.
 * This class is the base class of image processing executer.
 *****************************************************************************/


#ifndef CHOIMGPROC_H
#define CHOIMGPROC_H

#include "choimgproc_global.h"
#include <opencv2/opencv.hpp>
#include "ichoimgproc.h"
#include "basechoimgprocparam.h"

namespace choshiimgproc {

class CHOIMGPROCSHARED_EXPORT ChoImgProc {

 public:
  /* Constructor */
  ChoImgProc();
  ChoImgProc( const std::string &filename );
  /* Destructor */
  ~ChoImgProc();

  /* Accessor */
  bool setImage( const std::string &filename );
  bool setImage( cv::Mat *image );
  cv::Mat* getInputImage();
  cv::Mat* getDispImage();
  void setImgProc( const IChoImgProc *proc );

  /* Methods */
  bool init( BaseChoImgProcParam *param );
  bool clear();
  bool execute();
  ImgProcColorMode convertColor( ConvertColorMode eMode );
  void createDstImage();

 private:
  cv::Mat m_inputImage;
  cv::Mat m_dispImage;
  IChoImgProc *m_proc;

};  /* class ChoImgProc */

}   /* namespace choshiimgproc */

#endif // CHOIMGPROC_H
