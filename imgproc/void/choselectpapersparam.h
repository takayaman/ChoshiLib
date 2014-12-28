#ifndef CHOSELECTPAPERSPARAM_H
#define CHOSELECTPAPERSPARAM_H

#include <opencv2/opencv.hpp>
#include  "basechoimgprocparam.h"
#include "basepaperdatas.h"

namespace choshiimgproc {

class ChoSelectPapersParam : public BaseChoImgProcParam {
 public:
  ChoSelectPapersParam( BaseChoImgProcParam *param );
  virtual ~ChoSelectPapersParam();

  choshiresource::ChoCluster *getClusterAt( int labelNum );
  int getClusterSize( void );
  UsePapers getUsePapers( void );
  void setUsePapers( UsePapers ePapers );

  cv::Vec3d getPaperColor( int index );
  cv::Vec3d getPaperRGB( int index );
  cv::Vec3d getPaperHVC( int index );
  cv::Vec3d getPaperLAB( int index );
  int getPaperSize( void );
  std::string getPaperName( int index );



 private:
  cv::Mat *m_choshiImage;
  std::list <choshiresource::ChoCluster*> *m_clusterList;
  UsePapers m_usePaper;

}; /* class ChoSelectPapersParam */

} /* namespace choshiimgproc */

#endif // CHOSELECTPAPERSPARAM_H
