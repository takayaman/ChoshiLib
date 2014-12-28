#ifndef CHORAG_H
#define CHORAG_H

#include <opencv2/opencv.hpp>
#include "ichoimgproc.h"
#include "basechoimgprocparam.h"
#include "choragparam.h"

namespace choshiimgproc {

class ChoRAG : public IChoImgProc {

 private:
  enum MERGEWAY {
    MARGE_BASED_COLOR,
    MARGE_BASED_PIXNUM,
    NUM
  };

 public:
  ChoRAG();
  ~ChoRAG(); /* Virtual */

  bool init( BaseChoImgProcParam *param ); /* Virtual */
  bool clear(); /* Virtual */
  bool execute(); /* Virtual */
  // TODO settle to common function
  ImgProcColorMode convertColor( ConvertColorMode eMode ); /* Virtual */
  void createDstImage(); /* Virtual */

 private:
  void makeAdjacency();
  void clearAdjacency();
  void merge( MERGEWAY mergeway, cv::TermCriteria termCriTeria );

 private:
  ChoRAGParam *m_param;

}; /* class ChoRAG */

} /* namespace choshiimgproc */

#endif // CHORAG_H
