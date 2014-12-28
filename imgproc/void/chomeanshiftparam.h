#ifndef CHOMEANSHIFTPARAM_H
#define CHOMEANSHIFTPARAM_H

#include <opencv2/opencv.hpp>
#include "basechoimgprocparam.h"

namespace choshiimgproc {

class ChoMeanShiftParam : public BaseChoImgProcParam {
 public:
  ChoMeanShiftParam( BaseChoImgProcParam *param );
  virtual ~ChoMeanShiftParam();

  void setSpatialRadius( float sRadius );
  double getSpatialRadius( void );

  void setColorRadius( float cRadius );
  double getColorRadius( void );

  void setCvTermCriteria( cv::TermCriteria termCriteria );
  cv::TermCriteria getCvTermCriteria( void );

 private:
  float m_sRadius;
  float m_cRadius;
  cv::TermCriteria m_termCriteria;
  cv::Mat *m_choshiImage;

}; /* class ChoMeanShiftParam */

} /* namespace choshiimgproc */

#endif // CHOMEANSHIFTPARAM_H
