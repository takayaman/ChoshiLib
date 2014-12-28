/*=============================================================================
 * Project : ChoImgProc
 * Code : cholabeling.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class is execute labeling to each region.
 *===========================================================================*/

#ifndef CHOLABELING_H
#define CHOLABELING_H

#include <opencv2/opencv.hpp>
#include "./ichoimgproc.h"
#include "./basechoimgprocparam.h"
#include "./cholabelingparam.h"

namespace choshiimgproc {

/** This class is execute labeling to each region.
 */
class ChoLabeling : public IChoImgProc {
 public:
  /** Default constructor
   * @return None
   */
  ChoLabeling(void);

  /** Default destructor
   * @return None
   */
  ~ChoLabeling(void); /* Virtual */

  /** Initialize algorithm.
   * @param param Parameter of algorithm.
   * @return true : success, false : fail.
   * This method upcast param to ChoLabelingparam.
   */
  bool init(BaseChoImgProcParam *param); /* Virtual */

  /** 
   * @return true : success, false : fail.
   */
  bool clear(void); /* Virtual */

  /** 
   * @return true : success, false : fail.
   */
  bool execute(void); /* Virtual */
  // TODO settle to common function
  /** 
   * @param eMode Color mode to convert.
   * @return Color mode of converted.
   */
  ImgProcColorMode convertColor(ConvertColorMode eMode); /* Virtual */

  /** Create Destination image.
   */
  void createDstImage(void); /* Virtual */

 private:
  /** 
   * @return true : success, false : fail.
   */
  bool actionLabeling(void);

  /** 
   * @param ittPoint 
   * @param labelNum 
   * @return 
   */
  cv::Vec6d fill(cv::Point2i ittPoint, int labelNum);

 private:
  ChoLabelingParam *m_param;            /**< Parameters of algorithm. */
  int m_tempLabelCount;                 /**<  */
}; /* class ChoLabeling */

} /* namespace choshiimgproc */

#endif  // CHOLABELING_H
