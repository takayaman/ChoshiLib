/******************************************************************************
 * Project      : ChoResource
 * File         : chocluster.h
 * Copyright    : 2013 (c) Natsuki Takayama, UEC. Tokyo
 * The Header file of ChoCluster class.
 * This class holds cluster data.
 *****************************************************************************/


#ifndef CHOCLUSTER_H
#define CHOCLUSTER_H

#include "choresource_global.h"
#include <list>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace choshiresource {

class CHORESOURCESHARED_EXPORT ChoCluster {
 public:
  /* Constructor */
  ChoCluster();
  ChoCluster( ChoCluster *cluster );
  /* Destroctor */
  ~ChoCluster();

  /* Methods */
  /* handling adjacency cluster */
  void addAdjacencyCluster( ChoCluster *node );
  void deleteAdjacencyCluster( int labelNum );
  void clearAdjacencyClusterList();
  ChoCluster* seekAdjacencyCluster( int labelNum );
  bool absorbCluster( ChoCluster *cluster );
  bool checkAdjacencyPoint( cv::Point2i ittPt );
  void createAdjacencyInfo();
  void copyAdjacencyLabelNum( std::vector<int> *adjacencyLabelNums );


  bool isIsolatedConvex( cv::Mat *choshiImg );
  // TODO make ChoUtil Lib and move this function to util lib
  void writeMask( cv::Mat *srcChoshiImg, cv::Mat *dstMaskImg, int labelNum );
  void writeContour( cv::Mat *srcMaskImg, cv::Mat *dstContourImg );


 private:
  /* serialize */
  friend class boost::serialization::access;
  template <class Archive>
  void serialize( Archive &archive, unsigned int version ) {
    static_cast<void>( version );
    archive &boost::serialization::make_nvp( "meanX", m_meanXY.val[0] );
    archive &boost::serialization::make_nvp( "meanY", m_meanXY.val[1] );
    archive &boost::serialization::make_nvp( "meanB", m_meanRGB.val[0] );
    archive &boost::serialization::make_nvp( "meanG", m_meanRGB.val[1] );
    archive &boost::serialization::make_nvp( "meanR", m_meanRGB.val[2] );
    archive &boost::serialization::make_nvp( "meanH", m_meanHVC.val[0] );
    archive &boost::serialization::make_nvp( "meanV", m_meanHVC.val[1] );
    archive &boost::serialization::make_nvp( "meanC", m_meanHVC.val[2] );
    archive &boost::serialization::make_nvp( "meanL", m_meanLAB.val[0] );
    archive &boost::serialization::make_nvp( "meanA", m_meanLAB.val[1] );
    archive &boost::serialization::make_nvp( "meanB_lab", m_meanLAB.val[2] );
    archive &boost::serialization::make_nvp( "pixNum", m_holdPixelNum );
    archive &boost::serialization::make_nvp( "labelNum", m_labelNum );
    archive &boost::serialization::make_nvp( "paperName", m_paperName );
    archive &boost::serialization::make_nvp( "adjLabels", m_adjacencyLabelNums );
  }

 public:
  cv::Vec2i m_meanXY;
  cv::Vec3b m_meanRGB;
  cv::Vec3d m_meanHVC;
  cv::Vec3d m_meanLAB;
  int m_holdPixelNum;
  int m_labelNum;

  std::string m_paperName;

  std::list<ChoCluster*> m_adjacencyClusterList;
  std::list<cv::Point2i> m_adjacencyPointList;

 private:
  std::vector<int> m_adjacencyLabelNums;

}; /* class ChoCluster */

} /* namespace choshiresource */

#endif // CHOCLUSTER_H
