/*****************************************************************************/
// File         : CISP.cpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-1-19
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : GMMSP implementation
/*****************************************************************************/

#include "CISP.hpp"
#include <cfloat>
#include <algorithm>

namespace sp{

	void ranging(int &XXB, int &XXE, int &YYB, int &YYE, int W, int H, int n_x, int n_y, int k_x, int k_y, int v_x, int v_y, int t_x, int t_y, int rl, int ru){
		if ((k_x - t_x) <= 0)
			XXB = 0;
		else
			XXB = (k_x - t_x) * v_x + rl;

		if ((k_x + t_x) >= (n_x - 1))
			XXE = W;
		else
			XXE = (k_x + t_x + 1) * v_x + rl;

		if ((k_y - t_y) <= 0)
			YYB = 0;
		else
			YYB = (k_y - t_y) * v_y + ru;

		if ((k_y + t_y) >= (n_y - 1))
			YYE = H;
		else
			YYE = (k_y + t_y + 1) * v_y + ru;
	}

	void ranging(int &XXB, int &YYB, int k_x, int k_y, int v_x, int v_y, int t_x, int t_y, int rl, int ru){
		if ((k_x - t_x) <= 0)
			XXB = 0;
		else
			XXB = (k_x - t_x) * v_x + rl;

		if ((k_y - t_y) <= 0)
			YYB = 0;
		else
			YYB = (k_y - t_y) * v_y + ru;
	}


	inline void NSymE2x2(Real a00, Real a01, Real a11, Real &e0, Real &e1, Real &v00, Real &v01, Real &v10, Real &v11){
		Real const zero = (Real)0, one = (Real)1, half = (Real)0.5;
		Real c2 = half * (a00 - a11), s2 = a01;
		Real maxAbsComp = std::max(std::abs(c2), std::abs(s2));
		if (maxAbsComp > zero){
			c2 /= maxAbsComp;  // in [-1,1]
			s2 /= maxAbsComp;  // in [-1,1]
			Real length = sqrt(c2 * c2 + s2 * s2);
			c2 /= length;
			s2 /= length;
			if (c2 > zero){
				c2 = -c2;	s2 = -s2;
			}
		}
		else{
			c2 = -one; s2 = zero;
		}
		Real s = sqrt(half * (one - c2));  // >= 1/sqrt(2)
		Real c = half * s2 / s;

		Real csqr = c * c, ssqr = s * s, mid = s2 * a01;
		e0 = csqr * a00 + mid + ssqr * a11;
		e1 = csqr * a11 - mid + ssqr * a00;

		v00 = c;	v01 = s;
		v10 = -s; 	v11 = c;
	}

	void extract_labels(
		LabT *oL, int oL_steps,
		Real *oR, int oR_steps, int oR_lysize,
		int W, int H, int v_x, int v_y, int t_x, int t_y, int rl, int ru
	){
		int n_x = W / v_x;
		int n_y = H / v_y;

		int ntx = 2 * t_x + 1;
		int nty = 2 * t_y + 1;
#pragma omp parallel for
		for (int y = 0; y < H; y++){
			for (int x = 0; x < W; x++){
				int ilabel_x = (x - rl) / v_x; if (ilabel_x == n_x) ilabel_x = n_x - 1;
				int ilabel_y = (y - ru) / v_y; if (ilabel_y == n_y) ilabel_y = n_y - 1;

				Real max_dense = -FLT_MAX;
				int final_label = -1;

				for (int dy = -t_y; dy <= t_y; dy++){
					for (int dx = -t_x; dx <= t_x; dx++){
						const int al_x = ilabel_x + dx;
						const int al_y = ilabel_y + dy;
						if (al_x < 0 || al_y < 0 || al_x >= n_x || al_y >= n_y){
							continue;
						}
						const int al_k = al_y*n_x + al_x;

						int xxb, yyb;
						ranging(xxb, yyb, al_x, al_y, v_x, v_y, t_x, t_y, rl, ru);
						Real cur_dense = oR[(y - yyb)*oR_steps + x - xxb + al_k*oR_lysize];
						if (max_dense < cur_dense){
							max_dense = cur_dense;
							final_label = al_k;
						}
					}
				}
				oL[y*oL_steps + x] = final_label;
			}
		}
	}


	void update_theta(
		Real *MG, int MG_steps,
		PixI *iC, int iC_steps,
		Real *oR, int oR_steps, int oR_lysize,
		int W, int H, int v_x, int v_y, int t_x, int t_y, int rl, int ru,
		Real epsilon_s, Real epsilon_c
	){
		const int n_x = W / v_x;
		const int n_y = H / v_y;

#pragma omp parallel for
		for (int k_y = 0; k_y < n_y; k_y++){
			for (int k_x = 0; k_x < n_x; k_x++){
				int XXB, XXE, YYB, YYE;
				int ik = k_x + k_y * n_x;

				Real mx = 0, my = 0, ml = 0, ma = 0, mb = 0, md = 0;
				ranging(XXB, XXE, YYB, YYE, W, H, n_x, n_y, k_x, k_y, v_x, v_y, t_x, t_y, rl, ru);				

				for (int y = YYB; y < YYE; y++){
					for (int x = XXB; x < XXE; x++){

						const Real RV  = oR[(y - YYB)*oR_steps + (x - XXB) + ik*oR_lysize];
						const PixI pix = iC[y*iC_steps + x];
						mx += RV*x;
						my += RV*y;
						ml += RV*pix.f0();
						ma += RV*pix.f1();
						mb += RV*pix.f2();
						md += RV;
					}
				}
				if (md > 1){
					MG[ik*MG_steps + 0] = mx = mx / md; // x
					MG[ik*MG_steps + 1] = my = my / md; // y
					MG[ik*MG_steps + 2] = ml = ml / md; // l
					MG[ik*MG_steps + 3] = ma = ma / md; // a
					MG[ik*MG_steps + 4] = mb = mb / md; // b
					md = 1.f / md; // d
				}
				else{
					continue;
				}


				Real xy00 = 0, xy01 = 0, xy11 = 0;
				Real ab00 = 0, ab01 = 0, ab11 = 0;
				Real sl = 0;
				Real tp0, tp1;

				for (int y = YYB; y < YYE; y++){
					for (int x = XXB; x < XXE; x++){

						const Real RV = oR[(y - YYB)*oR_steps + x - XXB + ik*oR_lysize];
						const PixI pix = iC[y*iC_steps + x];

						tp0 = x - mx;
						tp1 = y - my;

						xy00 += RV * tp0 * tp0;
						xy01 += RV * tp0 * tp1;
						xy11 += RV * tp1 * tp1;

						tp0 = pix.f0() - ml;
						sl += RV * tp0 * tp0;

						tp0 = pix.f1() - ma;
						tp1 = pix.f2() - mb;
						ab00 += RV * tp0 * tp0;
						ab01 += RV * tp0 * tp1;
						ab11 += RV * tp1 * tp1;
					}
				}
				Real isx, isy, isl, isa, isb;
				Real vxy00, vxy01, vxy10, vxy11;
				Real vab00, vab01, vab10, vab11;
				Real isd;

				xy00 = xy00 * md;
				xy01 = xy01 * md;
				xy11 = xy11 * md;
				NSymE2x2(xy00, xy01, xy11, isx, isy, vxy00, vxy01, vxy10, vxy11);
				if (isx < epsilon_s) { isx = epsilon_s; }
				if (isy < epsilon_s) { isy = epsilon_s; }
				isx = 1.f / isx;
				isy = 1.f / isy;

				ab00 = ab00 * md;
				ab01 = ab01 * md;
				ab11 = ab11 * md;
				NSymE2x2(ab00, ab01, ab11, isa, isb, vab00, vab01, vab10, vab11);
				if (isa < epsilon_c) { isa = epsilon_c; }
				if (isb < epsilon_c) { isb = epsilon_c; }
				isa = 1.f / isa;
				isb = 1.f / isb;

				sl = sl * md;
				if (sl < epsilon_c){ sl = epsilon_c; }
				isl = 1.f / sl;

				isd = sqrt(isl * isx * isy * isa * isb);

				//////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////
				MG[ik*MG_steps +  0+5] = isx;
				MG[ik*MG_steps +  1+5] = isy;
				MG[ik*MG_steps +  2+5] = isl;
				MG[ik*MG_steps +  3+5] = isa;
				MG[ik*MG_steps +  4+5] = isb;
				MG[ik*MG_steps +  5+5] = vxy00;
				MG[ik*MG_steps +  6+5] = vxy01;
				MG[ik*MG_steps +  7+5] = vxy10;
				MG[ik*MG_steps +  8+5] = vxy11;
				MG[ik*MG_steps +  9+5] = vab00;
				MG[ik*MG_steps + 10+5] = vab01;
				MG[ik*MG_steps + 11+5] = vab10;
				MG[ik*MG_steps + 12+5] = vab11;
				MG[ik*MG_steps + 13+5] = isd; // all 14
			}
		}
	}

	void update_R(
		LabT     *iL, int iL_steps,
		Real     *oR, int oR_steps, int oR_lysize,
		su::PixI *iC, int iC_steps,
		Real     *MG, int MG_steps,
		int W, int H, int v_x, int v_y, int t_x, int t_y, int rl, int ru
	){
		int n_x = W / v_x;
		int n_y = H / v_y;
		int K = n_x * n_y;

		const int ntx = t_x * 2 + 1;
		const int nty = t_y * 2 + 1;
		const Real epsilon_t = sizeof(Real) == 4 ? (FLT_MIN*9.f) : (DBL_MIN*9.);

#pragma omp parallel for
		for (int y = 0; y < H; y++){
			for (int x = 0; x < W; x++){

				// mark for improve
				int ik = iL[y*iL_steps + x];
				int ik_y = ik / n_x;
				int ik_x = ik - ik_y*n_x;

				int ok;
				int ok_x, ok_y;

				Real ffi[2], ffo[2], ff;
				Real d_xy, d_l, d_ab, D;


				Real sum_R = 0;
				int sum_exists = 0;

				for (int dy = -t_y; dy <= t_y; dy++){
					for (int dx = -t_x; dx <= t_x; dx++){
						ok_x = ik_x + dx;
						ok_y = ik_y + dy;

						// Very important for using >= x_num and >= y_num
						if (ok_x < 0 || ok_x >= n_x || ok_y < 0 || ok_y >= n_y){
							continue;
						}

						ok = ok_y*n_x + ok_x;

						// read theta
						Real    mx = MG[ok*MG_steps +  0]; // x
						Real    my = MG[ok*MG_steps +  1]; // y
						Real    ml = MG[ok*MG_steps +  2]; // l
						Real    ma = MG[ok*MG_steps +  3]; // a
						Real    mb = MG[ok*MG_steps +  4]; // b
						Real   isx = MG[ok*MG_steps +  5]; // isx;
						Real   isy = MG[ok*MG_steps +  6]; // isy;
						Real   isl = MG[ok*MG_steps +  7]; // isl;
						Real   isa = MG[ok*MG_steps +  8]; // isa;
						Real   isb = MG[ok*MG_steps +  9]; // isb;
						Real vxy00 = MG[ok*MG_steps + 10]; // vxy00;
						Real vxy01 = MG[ok*MG_steps + 11]; // vxy01;
						Real vxy10 = MG[ok*MG_steps + 12]; // vxy10;
						Real vxy11 = MG[ok*MG_steps + 13]; // vxy11;
						Real vab00 = MG[ok*MG_steps + 14]; // vab00;
						Real vab01 = MG[ok*MG_steps + 15]; // vab01;
						Real vab10 = MG[ok*MG_steps + 16]; // vab10;
						Real vab11 = MG[ok*MG_steps + 17]; // vab11;
						Real   isd = MG[ok*MG_steps + 18]; // isd;

						// read pixel
						su::PixI px = iC[y*iC_steps + x];

						// x, y
						ffi[0] = x - mx; ffi[1] = y - my;
						ffo[0] = ffi[0] * vxy00 + ffi[1] * vxy01; ffo[0] = ffo[0] * ffo[0];
						ffo[1] = ffi[0] * vxy10 + ffi[1] * vxy11; ffo[1] = ffo[1] * ffo[1];
						d_xy = ffo[0] * isx + ffo[1] * isy;

						// l
						ff = px.f0() - ml; ff = ff*ff;
						d_l = ff * isl;

						// a, b
						ffi[0] = px.f1() - ma;
						ffi[1] = px.f2() - mb;
						ffo[0] = ffi[0] * vab00 + ffi[1] * vab01; ffo[0] = ffo[0] * ffo[0];
						ffo[1] = ffi[0] * vab10 + ffi[1] * vab11; ffo[1] = ffo[1] * ffo[1];
						d_ab = ffo[0] * isa + ffo[1] * isb;

						D = (d_xy + d_l + d_ab)*Real(-0.5);

						int xxb, yyb;
						ranging(xxb, yyb, ok_x, ok_y, v_x, v_y, t_x, t_y, rl, ru);
						oR[(y - yyb)*oR_steps + (x - xxb) + ok*oR_lysize] = exp(D) * isd;

						sum_R += exp(D) * isd;
						sum_exists++;
					}
				} ///

				if (sum_R < epsilon_t){
					for (int dy = -t_y; dy <= t_y; dy++){
						for (int dx = -t_x; dx <= t_x; dx++){
							ok_x = ik_x + dx;
							ok_y = ik_y + dy;
							if (ok_x < 0 || ok_x >= n_x || ok_y < 0 || ok_y >= n_y){
								continue;
							}
							ok = ok_y*n_x + ok_x;
							int xxb, yyb;
							ranging(xxb, yyb, ok_x, ok_y, v_x, v_y, t_x, t_y, rl, ru);
							oR[(y - yyb)*oR_steps + (x - xxb) + ok*oR_lysize] = Real(1) / Real(sum_exists);
						}
					}
				}
				else{
					for (int dy = -t_y; dy <= t_y; dy++){
						for (int dx = -t_x; dx <= t_x; dx++){
							ok_x = ik_x + dx;
							ok_y = ik_y + dy;
							if (ok_x < 0 || ok_x >= n_x || ok_y < 0 || ok_y >= n_y){
								continue;
							}
							ok = ok_y*n_x + ok_x;
							int xxb, yyb;
							ranging(xxb, yyb, ok_x, ok_y, v_x, v_y, t_x, t_y, rl, ru);
							Real &roR = oR[(y - yyb)*oR_steps + (x - xxb) + ok*oR_lysize];
							roR = roR / sum_R;
						}
					}
				}
			}
		}
	}

	void init_theta(Real *MG, int MG_steps, su::PixI *color, int color_steps, int W, int H, int v_x, int v_y, Real sl, Real sa, Real sb){
		int n_x = W / v_x;
		int n_y = H / v_y;
		int K = n_x * n_y;

		Real isx = (1.f) / (v_x*v_x); // x
		Real isy = (1.f) / (v_y*v_y); // y
		Real isl = (1.f) / (sl*sl); // l
		Real isa = (1.f) / (sa*sa); // a
		Real isb = (1.f) / (sb*sb); // b

		Real vxy00 = 1.f, vxy01 = 0.f; // direction on x
		Real vxy10 = 0.f, vxy11 = 1.f; // direction on y

		Real vab00 = 1.f, vab01 = 0.f; // direction on a
		Real vab10 = 0.f, vab11 = 1.f; // direction on b

		Real isd = sqrt(isx * isy * isl * isa * isb);

		int x_half_rest = (W - n_x*v_x) >> 1;
		int y_half_rest = (H - n_y*v_y) >> 1;
		int half_x_steps = v_x >> 1;
		int half_y_steps = v_y >> 1;

		for (int k_y = 0; k_y < n_y; k_y++){
			for (int k_x = 0; k_x < n_x; k_x++){
				int fx = (x_half_rest + k_x*v_x + half_x_steps);
				int fy = (y_half_rest + k_y*v_y + half_y_steps);
				int k = k_y * n_x + k_x;
				
				MG[k*MG_steps +  0] = fx;
				MG[k*MG_steps +  1] = fy;
				MG[k*MG_steps +  2] = color[fy*color_steps + fx].f0();
				MG[k*MG_steps +  3] = color[fy*color_steps + fx].f1();
				MG[k*MG_steps +  4] = color[fy*color_steps + fx].f2();
				MG[k*MG_steps +  5] = isx;
				MG[k*MG_steps +  6] = isy;
				MG[k*MG_steps +  7] = isl;
				MG[k*MG_steps +  8] = isa;
				MG[k*MG_steps +  9] = isb;
				MG[k*MG_steps + 10] = vxy00;
				MG[k*MG_steps + 11] = vxy01;
				MG[k*MG_steps + 12] = vxy10;
				MG[k*MG_steps + 13] = vxy11;
				MG[k*MG_steps + 14] = vab00;
				MG[k*MG_steps + 15] = vab01;
				MG[k*MG_steps + 16] = vab10;
				MG[k*MG_steps + 17] = vab11;
				MG[k*MG_steps + 18] = isd;
			}
		}
	}
	

	void init_labels(LabT *iL, int iL_steps, int W, int H, int v_x, int v_y, int n_x, int n_y, int rl, int ru){
		for (int y = 0; y < H; y++){
			for (int x = 0; x < W; x++){
				int k_x = (x - rl) / v_x;
				int k_y = (y - ru) / v_y;
				if (k_x < 0) k_x = 0;
				else if (k_x >(n_x - 1)) k_x = n_x - 1;

				if (k_y < 0) k_y = 0;
				else if (k_y >(n_y - 1)) k_y = n_y - 1;

				iL[y*iL_steps + x] = k_x + k_y*n_x;
			}
		}
	}

}// end namespace
