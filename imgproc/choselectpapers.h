/*=============================================================================
 * Project : ChoImgProc
 * Code : cholabeling.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class is execute changing color of region.
 *===========================================================================*/

#ifndef CHOSELECEPAPERS_H
#define CHOSELECEPAPERS_H

#include <opencv2/opencv.hpp>
#include "./ichoimgproc.h"
#include "./basechoimgprocparam.h"
#include "./choselectpapersparam.h"

namespace choshiimgproc {

/** This class executes to change color of regions.
 */
class ChoSelectPapers : public IChoImgProc {
 public:
  /** Default Constructor.
   * @return None.
   */
  ChoSelectPapers(void);

  /** Destructor.
   * @return None.
   */
  ~ChoSelectPapers(void); /* Virtual */

  /** 
   * @param param Parameters of algorithm.
   * @return true : success, false : fail.
   * This method upcasts param to ChoSelectPapersParam
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
  /** Run select paper algorithm.
   */
  void selectPapers(void);

 private:
  ChoSelectPapersParam *m_param;        /**< Parameter of algorithm. */
}; /* class ChoSelectPapers */

} /* namespace choshiimgproc */

#endif  // CHOSELECEPAPERS_H
