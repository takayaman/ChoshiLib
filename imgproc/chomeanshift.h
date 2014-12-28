/*=============================================================================
 * Project : ChoImgProc
 * Code : cholabeling.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class is execute mean shift algorithm to segement region.
 *===========================================================================*/

#ifndef CHOMEANSHIFT_H
#define CHOMEANSHIFT_H

#include <opencv2/opencv.hpp>

#include "./ichoimgproc.h"
#include "./basechoimgprocparam.h"
#include "./chomeanshiftparam.h"

namespace choshiimgproc {

/** This class is execute mean shift algorithm to segment region.
 */
class ChoMeanShift : public IChoImgProc {
 public:
  /** Default constructo.
   * @return None.
   */
  ChoMeanShift(void);

  /** Destructor
   * @return None.
   */
  ~ChoMeanShift(void); /* Virtual */

  /** 
   * @param param Pointer of parameters.
   * @return true : success, false : fail.
   * This method upcasts param to ChoMeanShiftParam.
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
  /** Run MeanShift segmentation.
   * @return true : success, false : fail.
   */
  bool meanshift(void);

  /** 
   * @param ittPt 
   * @param termCrit 
   * @param passedPoint 
   * @return 
   */
  CVVEC_CHOSHI actionMeanShift(cv::Point2i ittPt, cv::TermCriteria termCrit, cv::Point2i *passedPoint);

 private:
  ChoMeanShiftParam *m_param;           /**< Pointer of algorithm parameters. */
}; /* class ChoMeanShift */

} /* namespace choshiimgproc */

#endif  // CHOMEANSHIFT_H
