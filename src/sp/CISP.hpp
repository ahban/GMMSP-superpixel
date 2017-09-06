/*****************************************************************************/
// File         : CISP.hpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-1-19
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : GMMSP interfaces
/*****************************************************************************/

#ifndef __CISP_HPP__
#define __CISP_HPP__

#include "su/matrix.hpp"
#include "su/pixel.hpp"
#include "su/colors.hpp"


#include <inttypes.h>

namespace sp{
	// basic types for GMMSP
	typedef float   Real;
	typedef int32_t LabT;
	using su::PixI;

	typedef su::Mat<sp::Real> MatF; // theta, R
	typedef su::Mat<sp::LabT> MatL; // label type
	typedef su::Mat<su::PixI> MatP; // image 

	// interfaces
	void init_labels(LabT *iL, int iL_steps, int W, int H, int v_x, int v_y, int n_x, int n_y, int rl, int ru);
	void init_theta(Real *MG, int MG_steps, su::PixI *color, int color_steps, int W, int H, int v_x, int v_y, Real sl, Real sa, Real sb);
	void update_R(
		LabT *iL, int iL_steps,
		Real *oR, int oR_steps, int oR_lysize,
		PixI *iC, int iC_steps,
		Real *MG, int MG_steps,
		int W, int H, int v_x, int v_y, int t_x, int t_y, int rl, int ru
	);
	void update_theta(
		Real *MG, int MG_steps,
		PixI *iC, int iC_steps,
		Real *oR, int oR_steps, int oR_lysize,
		int W, int H, int v_x, int v_y, int t_x, int t_y, int rl, int ru,
		Real epsilon_s, Real epsilon_c
	);
	void extract_labels(
		LabT *oL, int oL_steps,
		Real *oR, int oR_steps, int oR_lysize,
		int W, int H, int v_x, int v_y, int t_x, int t_y, int rl, int ru
	);

	// class 
	class CISP{
	public:
    
		template<class _MP>
		void segmentation_S(_MP &bgr, int v_xi, int v_yi, int Ti, Real e_ci, Real e_si, Real lambdai){
			//////////////////////////////////////////////////////////////////////////
			// configurations
			W = bgr.width;
			H = bgr.height;
			lambda = lambdai;
			T = Ti;
			Real sl = lambda;
			Real sa = lambda;
			Real sb = lambda;

			e_c = e_ci;	e_s = e_si;
			v_x = v_xi; v_y = v_yi;

			t_x = 1;
			t_y = 1;

			int ntx = 2 * t_x + 1;
			int nty = 2 * t_y + 1;

			int n_x = W / v_x;
			int n_y = H / v_y;

			K = n_x * n_y;

			int reminder_W = W - n_x*v_x;
			int reminder_H = H - n_y*v_y;

			int rl = reminder_W >> 1;
			int ru = reminder_H >> 1;

			//////////////////////////////////////////////////////////////////////////
			// memory
			int avx = v_x*ntx + reminder_W;
			int avy = v_y*nty + reminder_H;

			iL.create(H, W);
			oL.create(H, W);
			
			oR.create(avy, avx, K);
			oR.clear();
			lab.create(H, W);
			MG.create(K, 19); // x, y, l, a, b, 1/d


			//////////////////////////////////////////////////////////////////////////
			// method
			su::bgr2lab(lab.data, lab.steps, bgr.data, bgr.steps, W, H);
			init_labels(iL.data, iL.steps, W, H, v_x, v_y, n_x, n_y, rl, ru);
			init_theta(MG.data, MG.steps, lab.data, lab.steps, W, H, v_x, v_y, sl, sa, sb);
			
			update_R(iL.data, iL.steps, oR.data, oR.steps, oR.steps*oR.rows, lab.data, lab.steps, MG.data, MG.steps, W, H, v_x, v_y, t_x, t_y, rl, ru);

			for (int it = 1; it < T; it++){
				update_theta(MG.data, MG.steps, lab.data, lab.steps, oR.data, oR.steps, oR.steps*oR.rows, W, H, v_x, v_y, t_x, t_y, rl, ru, e_s, e_c);
				update_R(iL.data, iL.steps, oR.data, oR.steps, oR.steps*oR.rows, lab.data, lab.steps, MG.data, MG.steps, W, H, v_x, v_y, t_x, t_y, rl, ru);
			}
			extract_labels(oL.data, oL.steps, oR.data, oR.steps, oR.steps*oR.rows, W, H, v_x, v_y, t_x, t_y, rl, ru);
		}

		void disp_configures(){
			std::cout << "image is " << W << "x" << H << std::endl;
			std::cout << "lambda = " << lambda << std::endl;
			std::cout << "   e_c = " << e_c << std::endl;
			std::cout << "   e_s = " << e_s << std::endl;
			std::cout << "   v_x = " << v_x << std::endl;
			std::cout << "   v_y = " << v_y << std::endl;
			std::cout << "   n_x = " << W / v_x << std::endl;
			std::cout << "   n_y = " << H / v_y << std::endl;
			std::cout << "     T = " << T << std::endl;
		}

	public:
		int W, H;

		int v_x, v_y, K;
		int t_x, t_y;
		int T;
		Real e_c, e_s;
		Real lambda;

	public:
		MatP lab; // CIELab color space
		MatL iL; // constant input label
		MatL oL; // output label
		MatF oR; // post probability of superpixel k for pixel i
		MatF MG; // $\theta$ theta
	};
} // end namespace su
#endif