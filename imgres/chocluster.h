/*=============================================================================
 * Project : ChoResource
 * Code : chocluster.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class holds cluster data.
 *===========================================================================*/

#ifndef CHOCLUSTER_H
#define CHOCLUSTER_H


#include <opencv2/opencv.hpp>

#include <list>
#include <fstream>
#include <string>
#include <vector>

#include "./choresource_global.h"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace choshiresource {

/** This class holds cluster data.
 */
class CHORESOURCESHARED_EXPORT ChoCluster {
 public:
  /** Default constructor.
   * @return None
   */
  ChoCluster(void);

  /** Copy constructor.
   * @param cluster
   * @return None
   * Exception of "explicit" to define sort algorithm.
   */
  ChoCluster(ChoCluster *cluster);

  /** Default destructor.
   * @return None
   */
  ~ChoCluster(void);

  /*--- Methods --------------------------------------------------------------*/
  /* handling adjacency cluster */

  /**
   * @param node Adjacency cluster to add.
   */
  void addAdjacencyCluster(ChoCluster *node);

  /**
   * @param labelNum Index of a cluster to delete.
   */
  void deleteAdjacencyCluster(int labelNum);

  /** Clear list of adjacency cluster list.
   */
  void clearAdjacencyClusterList(void);

  /**
   * @param labelNum Index of a cluster to seek.
   * @return
   */
  ChoCluster* seekAdjacencyCluster(int labelNum);

  /**
   * @param cluster Pointer of cluster which is merged to this cluster.
   * @return true : success, false : fail.
   */
  bool absorbCluster(ChoCluster *cluster);

  /**
   * @param ittPt
   * @return
   */
  bool checkAdjacencyPoint(cv::Point2i ittPt);

  /**
   */
  void createAdjacencyInfo(void);

  /**
   * @param adjacencyLabelNums
   */
  void copyAdjacencyLabelNum(std::vector<int> *adjacencyLabelNums);

  /**
   * @param choshiImg
   * @return
   */
  bool isIsolatedConvex(cv::Mat *choshiImg);
  // TODO make ChoUtil Lib and move this function to util lib

  /**
   * @param srcChoshiImg
   * @param dstMaskImg
   * @param labelNum
   */
  void writeMask(cv::Mat *srcChoshiImg, cv::Mat *dstMaskImg, int labelNum);

  /**
   * @param srcMaskImg
   * @param dstContourImg
   */
  void writeContour(cv::Mat *srcMaskImg, cv::Mat *dstContourImg);

  /* sort algorithm */
  /**
   * @param val1 Left hand side.
   * @param val2 Right hand side.
   * @return
   */
  static bool ascending(const ChoCluster& val1, const ChoCluster& val2) {
    if (val1.m_labelNum < val2.m_labelNum) {
      return true;
    } else {
      return false;
    }
  }

  static bool pAscending(const ChoCluster *val1, const ChoCluster *val2) {
      return val1->m_labelNum < val2->m_labelNum;
  }

 private:
  /* serialize */
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &archive, unsigned int version) {
    static_cast<void>(version);
    archive &boost::serialization::make_nvp("meanX", m_meanXY.val[0]);
    archive &boost::serialization::make_nvp("meanY", m_meanXY.val[1]);
    archive &boost::serialization::make_nvp("meanB", m_meanRGB.val[0]);
    archive &boost::serialization::make_nvp("meanG", m_meanRGB.val[1]);
    archive &boost::serialization::make_nvp("meanR", m_meanRGB.val[2]);
    archive &boost::serialization::make_nvp("meanH", m_meanHVC.val[0]);
    archive &boost::serialization::make_nvp("meanV", m_meanHVC.val[1]);
    archive &boost::serialization::make_nvp("meanC", m_meanHVC.val[2]);
    archive &boost::serialization::make_nvp("meanL", m_meanLAB.val[0]);
    archive &boost::serialization::make_nvp("meanA", m_meanLAB.val[1]);
    archive &boost::serialization::make_nvp("meanB_lab", m_meanLAB.val[2]);
    archive &boost::serialization::make_nvp("pixNum", m_holdPixelNum);
    archive &boost::serialization::make_nvp("labelNum", m_labelNum);
    archive &boost::serialization::make_nvp("paperName", m_paperName);
    archive &boost::serialization::make_nvp("adjLabels", m_adjacencyLabelNums);
  }

 public:
  cv::Vec2i m_meanXY;                   /**< Center of cluster X */
  cv::Vec3b m_meanRGB;                  /**< Center of cluster Y */
  cv::Vec3d m_meanHVC;                  /**< Center of cluster HVC */
  cv::Vec3d m_meanLAB;                  /**< Center of cluster LAB */
  int m_holdPixelNum;                   /**< The number of belonging pixels.*/
  int m_labelNum;                       /**< Index of region. */

  std::string m_paperName;              /**< Paper name to display. */

  std::list<ChoCluster*> m_adjacencyClusterList; /**< Region adjacency graphs. */
  std::list<cv::Point2i> m_adjacencyPointList; /**<  */

 private:
  std::vector<int> m_adjacencyLabelNums; /**<  */

}; /* class ChoCluster */

} /* namespace choshiresource */

#endif // CHOCLUSTER_H
