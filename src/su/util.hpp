/*****************************************************************************/
// File         : util.hpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-1-18
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : utility of su library for pure C/C++ or cuda application.
/*****************************************************************************/

#ifndef __SU_UTIL_H__
#define __SU_UTIL_H__

#include <stdlib.h>
#include <iostream>

#define ASSERTME 
#ifdef ASSERTME
  #define SU_ASSERT(ress, str) if (!(ress)) {std::cerr << "ERROR at " << __FILE__ << " : " << __LINE__ << " : " << str << std::endl; exit(EXIT_FAILURE);}
#else
  #define SU_ASSERT(ress, str)
#endif


#endif