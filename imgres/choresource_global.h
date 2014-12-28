/******************************************************************************
 * Project      : ChoResource
 * File         : choresource_global.h
 * Copyright    : 2013 (c) Natsuki Takayama, UEC. Tokyo
 * The Header file for exporting symbols.
 * This file is need for creating shared libraries.
 *****************************************************************************/

#ifndef CHORESOURCE_GLOBAL_H
#define CHORESOURCE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CHORESOURCE_LIBRARY)
#  define CHORESOURCESHARED_EXPORT Q_DECL_EXPORT
#else
#  define CHORESOURCESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CHORESOURCE_GLOBAL_H
