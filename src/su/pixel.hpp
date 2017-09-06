/*****************************************************************************/
// File         : pixel.hpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-1-19
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : pixel class
/*****************************************************************************/

#ifndef __PIXELI_HPP__
#define __PIXELI_HPP__

#include <inttypes.h>
#include <ostream>
#include <iomanip>

#ifdef __NVCC__
  #define QUALIFIER __device__ __host__ __forceinline__
#else
  #define QUALIFIER inline
#endif

namespace su{

	class PixI{
	public:
		uint32_t i;

		QUALIFIER PixI(const uint32_t &a){
			i = a;
		}
		QUALIFIER PixI(uint8_t c0, uint8_t c1, uint8_t c2){
			this->set(c0, c1, c2);
		}
		QUALIFIER PixI(){
			i = 0;
		}
		QUALIFIER float f0() const{ return (float)((i >> 0x10) & 0xFF); }
		QUALIFIER float f1() const{ return (float)((i >> 0x08) & 0xFF); }
		QUALIFIER float f2() const{ return (float)((i >> 0x00) & 0xFF); }

		QUALIFIER uint8_t u0() const{ return (uint8_t)((i >> 0x10) & 0xFF); }
		QUALIFIER uint8_t u1() const{ return (uint8_t)((i >> 0x08) & 0xFF); }
		QUALIFIER uint8_t u2() const{ return (uint8_t)((i >> 0x00) & 0xFF); }

		QUALIFIER void set(uint8_t c0, uint8_t c1, uint8_t c2){
			i  = (c2 << 0x00);
			i += (c1 << 0x08);
			i += (c0 << 0x10);
		}

		QUALIFIER PixI operator-(const PixI& B){
			PixI C;
			C.set(
				uint8_t(abs(f0() - B.f0())), 
				uint8_t(abs(f1() - B.f1())), 
				uint8_t(abs(f2() - B.f2()))
			);
			return C;
		}

		QUALIFIER PixI operator+(const PixI& B){
			PixI C;
			C.set(
				uint8_t(abs(f0() + B.f0())),
				uint8_t(abs(f1() + B.f1())),
				uint8_t(abs(f2() + B.f2()))
				);
			return C;
		}

		QUALIFIER friend double operator+=(double &R, const PixI& B){
			double tpr = B.f0(); tpr += B.f1();	tpr += B.f2();
			return tpr;
		}

		friend std::ostream& operator<<(std::ostream &output, const PixI &a){
			output << std::setw(3) << uint32_t(a.f0()) << ","
				     << std::setw(3) << uint32_t(a.f1()) << ","
				     << std::setw(3) << uint32_t(a.f2()) << "";
			return output;
		}
	};
} // end namespace

#endif