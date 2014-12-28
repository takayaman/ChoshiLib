/*=============================================================================
 * Project : ChoImgProc
 * Code : cholabelingparam.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class holds parameters for ChoRag class.
 *===========================================================================*/

#ifndef CHORAGPARAM_H
#define CHORAGPARAM_H

#include <opencv2/opencv.hpp>

#include <list>

#include "./basechoimgprocparam.h"
#include "./chocluster.h"

namespace choshiimgproc {

/** This class holds parameters for ChoRag class.
 */
class ChoRAGParam : public BaseChoImgProcParam {
 public:
  /** Constructor
   * @param param Parametr of algorithm.
   * @return None
   * This class upcasts param to ChoRAGParam.
   */
  explicit ChoRAGParam(BaseChoImgProcParam *param);

  /** Destructo
   * @return None
   */
  virtual ~ChoRAGParam(void);

  /** 
   * @param sRadius Spatial radius.
   */
  void setSpatialRadius(float sRadius);

  /** 
   * @return Spatial radius.
   */
  double getSpatialRadius(void);

  /** 
   * @param cRadius Color radius.
   */
  void setColorRadius(float cRadius);

  /** 
   * @return Color radius.
   */
  double getColorRadius(void);

  /** 
   * @param termCriteria Itteration parameter.
   */
  void setCvTermCriteria(cv::TermCriteria termCriteria);

  /** 
   * @return Itteration parameter.
   */
  cv::TermCriteria getCvTermCriteria(void);

  /** 
   * @param minPixNum Minimum pixel value to make region.
   */
  void setMinPixNum(int minPixNum);

  /** 
   * @return Minimum pixel value to make region.
   */
  int getClusterSize(void);

  /** 
   * @param labelNum Index of cluster.
   * @return Pointer of cluster.
   */
  choshiresource::ChoCluster *getClusterAt(int labelNum);

  /** Delete a cluster which is pointed by labelNum.
   * @param labelNum Index of cluster.
   * @return true : success, false : fail.
   */
  bool deleteClusterAt(int labelNum);

  /** 
   * @param cluster0 
   * @param cluster1 
   * @return 
   */
  cv::Vec2i mergeBasedOnColor(choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1);

  /** 
   * @param cluster0 
   * @param cluster1 
   * @return 
   */
  cv::Vec2i mergeBasedOnPixNum(choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1);

  /** Reassing label number to clusters.
   */
  void reassignLabelNum(void);

 private:
  /** 
   * @param cluster0 
   * @param cluster1 
   * @return 
   */
  cv::Vec2i merge(choshiresource::ChoCluster *cluster0, choshiresource::ChoCluster *cluster1);


 private:
  float m_sRadius;                      /**< Spatial radius. */
  float m_cRadius;                      /**< Color radius. */
  cv::TermCriteria m_termCriteria;      /**< Itteration parmeter. */
  cv::Mat *m_choshiImage;               /**< Choshi image. */
  int m_minPixNum;                      /**< Minimum pixels to make region. */
  std::list <choshiresource::ChoCluster*> *m_clusterList; /**< List of clusters. */
}; /* class ChoRAGParam */

} /* namespace choshiimgproc */

#endif  // CHORAGPARAM_H
