#ifndef CHOSELECEPAPERS_H
#define CHOSELECEPAPERS_H

#include <opencv2/opencv.hpp>
#include "ichoimgproc.h"
#include "basechoimgprocparam.h"
#include "choselectpapersparam.h"

namespace choshiimgproc {

class ChoSelectPapers : public IChoImgProc {
 public:
  ChoSelectPapers();
  ~ChoSelectPapers(); /* Virtual */

  bool init( BaseChoImgProcParam *param ); /* Virtual */
  bool clear(); /* Virtual */
  bool execute(); /* Virtual */
  // TODO settle to common function
  ImgProcColorMode convertColor( ConvertColorMode eMode ); /* Virtual */
  void createDstImage(); /* Virtual */

 private:
  void selectPapers();

 private:
  ChoSelectPapersParam *m_param;

}; /* class ChoSelectPapers */

} /* namespace choshiimgproc */

#endif // CHOSELECEPAPERS_H
