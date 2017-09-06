/*****************************************************************************/
// File         : memory.hpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-1-18
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : alignement memory management for pure C/C++
/*****************************************************************************/


#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__


#include <stdlib.h>
#ifdef _WIN32
#include <malloc.h>
#endif
#include <iostream>
#include <assert.h>


// default memory aligned size in bytes!
// must be a multiple of word size, e.g. you cannot use word with size 3, 5, 129
// etc. if you use your own structure, please use vector instead of any Mat, Hat,
// or Dat.!!!
#define _DEFAULT_AIGNED_WIDTH_BYTES 128

// defualt start address alignment, must be an integer power of 2, e.g. 2^1, 2^5.
#define _DEFAULT_AIGNED_BEGIN_BYTES 128


namespace su{

	template<class _T, size_t _S/*in bytes*/>	inline 
	size_t aligned_words(size_t nwords){
		assert(_S%sizeof(_T)==0 && _S>=sizeof(_T));
		return (((nwords*sizeof(_T)+_S - 1) / _S)*_S) / sizeof(_T);
	}

	class Mem
	{
	public:
#ifdef _WIN32
		/*!
		\param sz, Size of the requested memory allocation.
		\param ss, The alignment value, which must be an integer power of 2.
		*/
		static void *malloc(size_t sz, size_t ss = _DEFAULT_AIGNED_BEGIN_BYTES){
			return _aligned_malloc(sz, ss);
		}

		/*!
		\param pd, Pointer to release.
		*/
		static void  release(void *pd){
			_aligned_free(pd);
		}

#else
		/*!
		\param sz, Size of the requested memory allocation.
		\param ss, The alignment value, which must be an integer power of 2.
		*/
		static void *malloc(size_t sz, size_t ss = _DEFAULT_AIGNED_BEGIN_BYTES){
			void *pd;
			posix_memalign(&pd, ss, sz);
			return pd;
		}

		/*!
		\param pd, Pointer to release.
		*/
		static void release(void *pd){
			free(pd);
		}
#endif
	}; // end class Mem


} // end namespace
#endif