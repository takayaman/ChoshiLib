/*=============================================================================
 * Project : ChoImgProc
 * Code : cholabelingparam.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class holds parameters for ChoMeanShift class.
 *===========================================================================*/

#ifndef CHOMEANSHIFTPARAM_H
#define CHOMEANSHIFTPARAM_H

#include <opencv2/opencv.hpp>
#include "./basechoimgprocparam.h"

namespace choshiimgproc {

/** This class holds parameters for ChoMeanShift class.
 */
class ChoMeanShiftParam : public BaseChoImgProcParam {
 public:
  /** Constructor
   * @param param Parameters of algorighm.
   * @return None
   * This method upcasts param to ChoMeanShiftParam.
   */
  explicit ChoMeanShiftParam(BaseChoImgProcParam *param);

  /** Destructor
   * @return None.
   */
  virtual ~ChoMeanShiftParam(void);

  /** 
   * @param sRadius Spatial radius.
   */
  void setSpatialRadius(float sRadius);

  /** 
   * @return Spatial radius.
   */
  double getSpatialRadius(void);

  /** 
   * @param cRadius Color radius.
   */
  void setColorRadius(float cRadius);

  /** 
   * @return Color radius.
   */
  double getColorRadius(void);

  /** 
   * @param termCriteria Itteration parameter.
   */
  void setCvTermCriteria(cv::TermCriteria termCriteria);

  /** 
   * @return Itteration parameter.
   */
  cv::TermCriteria getCvTermCriteria(void);

 private:
  float m_sRadius;                      /**< Spatial radius. */
  float m_cRadius;                      /**< Color radius. */
  cv::TermCriteria m_termCriteria;      /**< Itteration parameter. */
  cv::Mat *m_choshiImage;               /**< Choshi image. */

}; /* class ChoMeanShiftParam */

} /* namespace choshiimgproc */

#endif  // CHOMEANSHIFTPARAM_H
