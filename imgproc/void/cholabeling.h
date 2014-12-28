#ifndef CHOLABELING_H
#define CHOLABELING_H

#include <opencv2/opencv.hpp>
#include "ichoimgproc.h"
#include "basechoimgprocparam.h"
#include "cholabelingparam.h"

namespace choshiimgproc {

class ChoLabeling : public IChoImgProc {
 public:
  ChoLabeling();
  ~ChoLabeling(); /* Virtual */

  bool init( BaseChoImgProcParam *param ); /* Virtual */
  bool clear(); /* Virtual */
  bool execute(); /* Virtual */
  // TODO settle to common function
  ImgProcColorMode convertColor( ConvertColorMode eMode ); /* Virtual */
  void createDstImage(); /* Virtual */

 private:
  bool actionLabeling();
  cv::Vec6d fill( cv::Point2i ittPoint, int labelNum );

 private:
  ChoLabelingParam *m_param;
  int m_tempLabelCount;

}; /* class ChoLabeling */

} /* namespace choshiimgproc */

#endif // CHOLABELING_H
