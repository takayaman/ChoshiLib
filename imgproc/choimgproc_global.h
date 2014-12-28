/******************************************************************************
 * Project      : ChoImgProc
 * File         : choimgproc_global.h
 * Copyright    : 2013 (c) Natsuki Takayama, UEC. Tokyo
 * The Header file for exporting symbols.
 * This file is need for creating shared libraries.
 *****************************************************************************/


#ifndef CHOIMGPROC_GLOBAL_H
#define CHOIMGPROC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CHOIMGPROC_LIBRARY)
#  define CHOIMGPROCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CHOIMGPROCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CHOIMGPROC_GLOBAL_H
