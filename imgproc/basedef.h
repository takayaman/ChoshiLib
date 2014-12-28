/*=============================================================================
 * Project : ChoImgProc
 * Code : basedef.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * Common definition.
 *===========================================================================*/

#ifndef BASEDEF_H
#define BASEDEF_H


namespace choshiimgproc {

#define CVVEC_CHOSHI cv::Vec6d          /**< The number of channels */

/** Color mode.
 */
typedef enum ImgProcColorMode_TAG {
  COLORMODE_RGB = 0,                    /**< RGB */
  COLORMODE_LAB,                        /**< LAB */
  COLORMODE_LAB10,                      /**< LAB under XYZ10 */
  COLORMODE_HVC,                        /**< HVC(MTM) */
  COLORMODE_HVC10,                      /**< HVC(MTM) under XYZ10 */
  COLORMODE_NUM
} ImgProcColorMode;

/* return num of elements */
template<typename TYPE, std::size_t SIZE>
std::size_t arrayLength(const TYPE (&)[SIZE]) {
  return SIZE;
}

/* pointer cast */
template <class T>
inline T pointer_cast(void* p) {
  return static_cast<T>(p);
}

} /* namespace choshiimgproc */

#endif  // BASEDEF_H
