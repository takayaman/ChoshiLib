/*=============================================================================
 * Project : ChoImgProc
 * Code : ichoimgproc.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class is the interface class of image processing executer.
 *===========================================================================*/

#ifndef ICHOIMGPROC_H
#define ICHOIMGPROC_H

#include "./basechoimgprocparam.h"
#include "./basedef.h"

namespace choshiimgproc {

/** Mode to convert color.
 */
typedef enum ConvertColorMode_TAG {
  RGB2LAB = 0,                          /**< RGB -> LAB */
  RGB2LAB10,                            /**< RGB -> LAB10 */
  RGB2HVC,                              /**< RGB -> HVC */
  RGB2HVC10,                            /**< RGB -> HVC10 */
  LAB2RGB,                              /**< LAB -> RGB */
  LAB102RGB,                            /**< LAB10 -> RGB */
  HVC2RGB,                              /**< HVC -> RGB */
  HVC102RGB,                            /**< HVC10 -> RGB */
  CONVERTMODE_NUM
} ConvertColorMode;

/** This class is the interface of image processing executer.
 */
class CHOIMGPROCSHARED_EXPORT IChoImgProc {
 public:
  /** Default constructor.
   * @return None.
   */
  IChoImgProc(void);

  /** Destructor.
   * @return None.
   */
  virtual ~IChoImgProc(void);

  /*--- Methods --------------------------------------------------------------*/

  /** 
   * @param param Parameter of algorithm
   * @return true : success, false : fail.
   */
  virtual bool init(BaseChoImgProcParam *param) = 0;

  /** 
   * @return true : success, false : fail
   */
  virtual bool clear(void) = 0;

  /** 
   * @return true : success, false : fail
   */
  virtual bool execute(void) = 0;

  /** 
   * @param eMode Color mode to convert.
   * @return Color mode of converted.
   */
  virtual ImgProcColorMode convertColor(ConvertColorMode eMode) = 0;

  /** Create Destination image.
   */
  virtual void createDstImage(void);

 private:
  BaseChoImgProcParam *m_param;         /**< Parameters of algorithm. */

}; /* class IChoImgProc */

} /* namespace choshiimgproc */

#endif  // ICHOIMGPROC_H
