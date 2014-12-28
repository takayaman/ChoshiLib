/*=============================================================================
 * Project : ChoImgProc
 * Code : cholabeling.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class executes construction of region adjacency graph for controling
 * regions.
 *===========================================================================*/

#ifndef CHORAG_H
#define CHORAG_H

#include <opencv2/opencv.hpp>
#include "./ichoimgproc.h"
#include "./basechoimgprocparam.h"
#include "./choragparam.h"

namespace choshiimgproc {

/** This class executes construction of region adjacency graph for controlling regions.
 */
class ChoRAG : public IChoImgProc {
 private:
  /** The Way of merging regions.
   */
  enum MERGEWAY {
    MARGE_BASED_COLOR,                  /**< Merge based on color. */
    MARGE_BASED_PIXNUM,                 /**< Merge besed on the number of pixels */
    NUM
  };

 public:
  /** Default Constructor.
   * @return None.
   */
  ChoRAG(void);

  /** Destructor.
   * @return None
   */
  ~ChoRAG(void); /* Virtual */

  /** 
   * @param param Parameter of algorithm.
   * @return true : success, false : fail.
   * This method upcasts param to ChoRAGParam.
   */
  bool init(BaseChoImgProcParam *param); /* Virtual */

  /** 
   * @return true : success, false : fail.
   */
  bool clear(void); /* Virtual */

  /** 
   * @return true : success, false : fail.
   */
  bool execute(void); /* Virtual */
  // TODO settle to common function

  /** 
   * @param eMode Color mode to convert.
   * @return Color mode of converted.
   */
  ImgProcColorMode convertColor(ConvertColorMode eMode); /* Virtual */

  /** Create Destination image.
   */
  void createDstImage(void); /* Virtual */

 private:

  /** Make adjacency region.
   */
  void makeAdjacency(void);

  /** Clear adjacency region.
   */
  void clearAdjacency(void);

  /** 
   * @param mergeway The way of merging region.
   * @param termCriTeria Itteration parameter.
   */
  void merge(MERGEWAY mergeway, cv::TermCriteria termCriTeria);

 private:
  ChoRAGParam *m_param;                 /**< Parameters of algorithm. */
}; /* class ChoRAG */

} /* namespace choshiimgproc */

#endif  // CHORAG_H
