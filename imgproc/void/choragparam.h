#ifndef CHORAGPARAM_H
#define CHORAGPARAM_H

#include <opencv2/opencv.hpp>
#include "basechoimgprocparam.h"
#include "chocluster.h"

namespace choshiimgproc {

class ChoRAGParam : public BaseChoImgProcParam {
 public:
  ChoRAGParam( BaseChoImgProcParam *param );
  virtual ~ChoRAGParam();

  void setSpatialRadius( float sRadius );
  double getSpatialRadius( void );
  void setColorRadius( float cRadius );
  double getColorRadius( void );
  void setCvTermCriteria( cv::TermCriteria termCriteria );
  cv::TermCriteria getCvTermCriteria( void );
  void setMinPixNum( int minPixNum );
  int getClusterSize( void );


  choshiresource::ChoCluster *getClusterAt( int labelNum );
  bool deleteClusterAt( int labelNum );
  cv::Vec2i mergeBasedOnColor( choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1 );
  cv::Vec2i mergeBasedOnPixNum( choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1 );
  void reassignLabelNum();

 private:
  cv::Vec2i merge( choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1 );


 private:
  float m_sRadius;
  float m_cRadius;
  cv::TermCriteria m_termCriteria;
  cv::Mat *m_choshiImage;
  int m_minPixNum;
  std::list <choshiresource::ChoCluster*> *m_clusterList;


}; /* class ChoRAGParam */

} /* namespace choshiimgproc */

#endif // CHORAGPARAM_H
