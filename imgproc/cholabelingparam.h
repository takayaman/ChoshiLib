/*=============================================================================
 * Project : ChoImgProc
 * Code : cholabelingparam.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class holds parameters for ChoLabeling class.
 *===========================================================================*/

#ifndef CHOLABELINGPARAM_H
#define CHOLABELINGPARAM_H

#include <opencv2/opencv.hpp>

#include <list>

#include "./basechoimgprocparam.h"
#include "./chocluster.h"

namespace choshiimgproc {

/** Mode for searching neighbor pixels.
 */
typedef enum NEIGHBORS_MODE_TAG {
  FOUR_NEIGHBORS = 0,                   /**< 4-neighbors */
  EIGHT_NEIGHBORS,                      /**< 8-neighbors */
  NEIGHBORS_MODENUM
} Neighbours_mode;

/** This class holds parameters for ChoLabeling class.
 */
class ChoLabelingParam : public BaseChoImgProcParam {
 public:
  /** 
   * @param param Pointer of parameters.
   * @return None.
   * This class is upcast param to ChoLabelingParam
   */
  explicit ChoLabelingParam(BaseChoImgProcParam *param);

  /** Destructor.
   * @return None.
   */
  virtual ~ChoLabelingParam(void);

  /** 
   * @param cluster Pointer of cluster.
   */
  void addCluster(choshiresource::ChoCluster *cluster);

  /** 
   * @param cRadius Color radius.
   */
  void setColorRadius(float cRadius);

  /** 
   * @return Color radius.
   */
  float getColorRadius(void);

  /** 
   * @return Mode for searching neighbor pixels.
   */
  Neighbours_mode getNeighborsMode(void);

  /** 
   * @param eMode Mode for searching neighbor pixels.
   */
  void setNeighborsMode(Neighbours_mode eMode);

 private:
  float m_cRadius;                      /**< Color radius. */
  cv::Mat *m_choshiImage;               /**< Choshi image. */
  bool m_chechAdjacency;                /**<  */
  std::list <choshiresource::ChoCluster*> *m_clusterList; /**< List of clusters. */
  Neighbours_mode m_neighborsMode;      /**< Mode for searching neighbor pixels. */

}; /* class ChoLabelingParam */

} /* namespace choshiimgproc */

#endif  // CHOLABELINGPARAM_H
