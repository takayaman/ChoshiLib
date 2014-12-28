#ifndef CHOLABELINGPARAM_H
#define CHOLABELINGPARAM_H

#include <opencv2/opencv.hpp>
#include "basechoimgprocparam.h"
#include "chocluster.h"

namespace choshiimgproc {

typedef enum NEIGHBORS_MODE_TAG {
  FOUR_NEIGHBORS = 0,
  EIGHT_NEIGHBORS,
  NEIGHBORS_MODENUM
} Neighbours_mode;

class ChoLabelingParam : public BaseChoImgProcParam {

 public:
  ChoLabelingParam( BaseChoImgProcParam *param );
  virtual ~ChoLabelingParam();

  void addCluster( choshiresource::ChoCluster *cluster );
  void setColorRadius( float cRadius );
  float getColorRadius( void );
  Neighbours_mode getNeighborsMode( void );
  void setNeighborsMode( Neighbours_mode eMode );

 private:
  float m_cRadius;
  cv::Mat *m_choshiImage;
  bool m_chechAdjacency;
  std::list <choshiresource::ChoCluster*> *m_clusterList;
  Neighbours_mode m_neighborsMode;

}; /* class ChoLabelingParam */

} /* namespace choshiimgproc */

#endif // CHOLABELINGPARAM_H
