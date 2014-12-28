/*=============================================================================
 * Project : ChoImgProc
 * Code : cholabelingparam.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class holds parameters for ChoSelectPapers class.
 *===========================================================================*/

#ifndef CHOSELECTPAPERSPARAM_H
#define CHOSELECTPAPERSPARAM_H

#include <opencv2/opencv.hpp>

#include <string>
#include <list>

#include "./basechoimgprocparam.h"
#include "./basepaperdatas.h"

namespace choshiimgproc {

/** This class holds parameters for ChoSelectPapers class.
 */
class ChoSelectPapersParam : public BaseChoImgProcParam {
 public:
  /** Constructor.
   * @param param Parameter of algorithm.
   * @return None.
   * This method upcasts param to ChoSelectPapersParam.
   */
  explicit ChoSelectPapersParam(BaseChoImgProcParam *param);

  /** Destructor.
   * @return None.
   */
  virtual ~ChoSelectPapersParam(void);

  /** 
   * @param labelNum Index of cluster.
   * @return None.
   */
  choshiresource::ChoCluster *getClusterAt(int labelNum);

  /** 
   * @return Cluster size.
   */
  int getClusterSize(void);

  /** 
   * @return Type of paper set.
   */
  UsePapers getUsePapers(void);

  /** 
   * @param ePaper Type of paper set.
   */
  void setUsePapers(UsePapers ePapers);

  /** 
   * @param index Index of paper.
   * @return 
   */
  cv::Vec3d getPaperColor(int index);

  /** 
   * @param index Index of paper.
   * @return 
   */
  cv::Vec3d getPaperRGB(int index);

  /** 
   * @param index Index of paper.
   * @return 
   */
  cv::Vec3d getPaperHVC(int index);

  /** 
   * @param index Index of paper.
   * @return 
   */
  cv::Vec3d getPaperLAB(int index);

  /** 
   * @return Size of paper set.
   */
  int getPaperSize(void);

  /** 
   * @param index Index of paper.
   * @return Paper name.
   */
  std::string getPaperName(int index);

 private:
  cv::Mat *m_choshiImage;               /**< Choshi image. */
  std::list <choshiresource::ChoCluster*> *m_clusterList; /**< List of cluster. */
  UsePapers m_usePaper;                 /**< Type of paper set to use. */

}; /* class ChoSelectPapersParam */

} /* namespace choshiimgproc */

#endif  // CHOSELECTPAPERSPARAM_H
