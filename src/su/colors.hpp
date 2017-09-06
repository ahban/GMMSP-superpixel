/*****************************************************************************/
// File         : colors.hpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-1-19
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : color operations
/*****************************************************************************/

#ifndef __COLORS_HPP__
#define __COLORS_HPP__

#include "pixel.hpp"
#include "colors.h"

namespace su{
	
  static void bgr2lab(PixI *lab, int lab_steps, PixI* bgr, int bgr_steps, int W, int H){
		for (int y = 0; y < H; y++){
			for (int x = 0; x < W; x++){

				float R = normalizer__*(bgr[y*bgr_steps + x]).f2(); // R
				float G = normalizer__*(bgr[y*bgr_steps + x]).f1(); // G
				float B = normalizer__*(bgr[y*bgr_steps + x]).f0(); // B

				if (R <= 0.04045f)	R = R / 12.92f;
				else				R = std::pow((R + 0.055f) / 1.055f, 2.4f);
				if (G <= 0.04045f)	G = G / 12.92f;
				else				G = std::pow((G + 0.055f) / 1.055f, 2.4f);
				if (B <= 0.04045f)	B = B / 12.92f;
				else				B = std::pow((B + 0.055f) / 1.055f, 2.4f);

				float X = R*0.412453f + G*0.357580f + B*0.180423f;
				float Y = R*0.212671f + G*0.715160f + B*0.072169f;
				float Z = R*0.019334f + G*0.119193f + B*0.950227f;

				Y = Y * iYr__;
				Z = Z * iZr__;
				X = X * iXr__;

				if (X > epsilon__)	X = std::pow(X, 1.f / 3.f);
				else			 X = (kappa__*X + 16.f) / 116.f;
				if (Y > epsilon__)	Y = std::pow(Y, 1.f / 3.f);
				else			 Y = (kappa__*Y + 16.f) / 116.f;
				if (Z > epsilon__)	Z = std::pow(Z, 1.f / 3.f);
				else			 Z = (kappa__*Z + 16.f) / 116.f;

#define CL (uint32_t)((116.f*Y - 16.f))
#define CA (uint32_t)(500.f*(X - Y) + 128.f + 0.5f)
#define CB (uint32_t)(200.f*(Y - Z) + 128.f + 0.5f)

				lab[y*lab_steps + x].set(CL, CA, CB);

#undef CL
#undef CA
#undef CB
			}
		}
	}

} // end namespace

#endif