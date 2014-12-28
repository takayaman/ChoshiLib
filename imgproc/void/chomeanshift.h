#ifndef CHOMEANSHIFT_H
#define CHOMEANSHIFT_H

#include <opencv2/opencv.hpp>
#include "ichoimgproc.h"
#include "basechoimgprocparam.h"
#include "chomeanshiftparam.h"

namespace choshiimgproc {

class ChoMeanShift : public IChoImgProc {
 public:
  ChoMeanShift();
  ~ChoMeanShift(); /* Virtual */

  bool init( BaseChoImgProcParam *param ); /* Virtual */
  bool clear(); /* Virtual */
  bool execute(); /* Virtual */
  // TODO settle to common function
  ImgProcColorMode convertColor( ConvertColorMode eMode ); /* Virtual */
  void createDstImage(); /* Virtual */

 private:
  bool meanshift();
  CVVEC_CHOSHI actionMeanShift( cv::Point2i ittPt, cv::TermCriteria termCrit  ,cv::Point2i *passedPoint );


 private:
  ChoMeanShiftParam *m_param;

}; /* class ChoMeanShift */

} /* namespace choshiimgproc */

#endif // CHOMEANSHIFT_H
