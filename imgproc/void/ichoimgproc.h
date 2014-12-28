/******************************************************************************
 * Project      : ChoImgProc
 * File         : ichoimgproc.h
 * Copyright    : 2013 (c) Natsuki Takayama, UEC. Tokyo
 * The Header file of IChoImgProc class.
 * This class is the interface class of image processing executer.
 *****************************************************************************/


#ifndef ICHOIMGPROC_H
#define ICHOIMGPROC_H

#include "basechoimgprocparam.h"
#include "basedef.h"

namespace choshiimgproc {

typedef enum ConvertColorMode_TAG {
  RGB2LAB = 0,
  RGB2LAB10,
  RGB2HVC,
  RGB2HVC10,
  LAB2RGB,
  LAB102RGB,
  HVC2RGB,
  HVC102RGB,
  CONVERTMODE_NUM
} ConvertColorMode;

/* Choshi Image Processing Interface Class */
class CHOIMGPROCSHARED_EXPORT IChoImgProc {
 public:
  /* Constructor */
  IChoImgProc();
  /* Destructor */
  virtual ~IChoImgProc();

  /* Methods */
  virtual bool init( BaseChoImgProcParam *param ) = 0;
  virtual bool clear() = 0;
  virtual bool execute() = 0;
  virtual ImgProcColorMode convertColor( ConvertColorMode eMode ) = 0;
  virtual void createDstImage();

 private:
  BaseChoImgProcParam *m_param;

}; /* class IChoImgProc */

} /* namespace choshiimgproc */

#endif // ICHOIMGPROC_H
