

#include "sp/CISP.hpp"
#include "sp/connectivity.hpp"
#include "sp/tools.hpp"

#include "su/pixel.hpp"

#include "mex.h"
#include "matrix.h"


#include <string>
#include <inttypes.h>

#include <fstream>
using namespace std;


void cvtmatp(sp::MatP &ou, uint8_t *im, int W, int H){
  ou.create(H, W);
  for (int x = 0; x < W; x++){
    for (int y = 0; y < H; y++){
      uint8_t pixR = im[y+x*H+0*W*H];
      uint8_t pixG = im[y+x*H+1*W*H];
      uint8_t pixB = im[y+x*H+2*W*H];
      ou(y,x) = su::PixI(pixB,pixG,pixR);
    }
  }
}

void cvtmatl(double *ol, sp::MatL &il, int W, int H){
  for (int x = 0; x < W; x++){
    for (int y = 0; y < H; y++){
      ol[y+x*H] = il(y, x);
    }
  }
}

// Basic useage
//[labels] = mx_GMMSP(image)
//[labels] = mx_GMMSP(image, v_x)
//[labels] = mx_GMMSP(image, v_x, v_y)
// Professional usage
//[labels] = mx_GMMSP(image, v_x, v_y, e_c)
//[labels] = mx_GMMSP(image, v_x, v_y, e_c, T)
//[labels] = mx_GMMSP(image, v_x, v_y, e_c, T, lambda)
//[labels] = mx_GMMSP(image, v_x, v_y, e_c, T, lambda, e_s)
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
  int v_x = 26;
	int v_y = 27;
  
  int T = 10;
	float e_c = 8;
	float e_s = 2;
	float lamb = 8;  
  
  if (nrhs < 1 || nrhs > 7){
    mexPrintf("Basic Usage :\n");
    mexPrintf("       [labels] = mx_GMMSP(image)\n");
    mexPrintf("       [labels] = mx_GMMSP(image, v_x)\n");
    mexPrintf("       [labels] = mx_GMMSP(image, v_x, v_y)\n\n");
    
    mexPrintf("Professional Usage :\n");
    mexPrintf("  Please make sure that you know exactly the effects of the professional parameters. \n  Otherwise, please keep their default values:\n");
    mexPrintf("       [labels] = mx_GMMSP(image, v_x, v_y, e_c)\n");
    mexPrintf("       [labels] = mx_GMMSP(image, v_x, v_y, e_c, T)\n");
    mexPrintf("       [labels] = mx_GMMSP(image, v_x, v_y, e_c, T, lambda)\n");
    mexPrintf("       [labels] = mx_GMMSP(image, v_x, v_y, e_c, T, lambda, e_s)\n\n");
    
    mexErrMsgTxt("Wrong usage!");
  }
  
  sp::MatP imgo;
  
  // check image
  if (!mxIsUint8(prhs[0])){
    mexErrMsgTxt("input image must be uint8!");
  }
  const mwSize  ndim = mxGetNumberOfDimensions(prhs[0]);
  const mwSize *dims = mxGetDimensions(prhs[0]);
  if (ndim != 3 || dims[2] != 3){
    mexErrMsgTxt("input image must be RGB color image!");
  }
  int W = dims[1];
  int H = dims[0];
  
  
  
  // Basic parameters
  if (nrhs > 1){
    double tpvs = mxGetScalar(prhs[1]);
    if (floor(tpvs)!=tpvs || tpvs < 8){
      mexErrMsgTxt("v_x must be an integer that is greater than or equal to 8!");
    }
    v_x = v_y = tpvs;
  }
  
  if (nrhs > 2){
    double tpvs = mxGetScalar(prhs[2]);
    if (floor(tpvs)!=tpvs || tpvs < 8){
      mexErrMsgTxt("v_y must be an integer that is greater than or equal to 8!");
    }
    v_y = tpvs;
  }  
  
  // professonal parameters
  if (nrhs > 3){
    double tpvs;
    tpvs = mxGetScalar(prhs[3]);
    if (tpvs <= 0){
      mexErrMsgTxt("e_c must be greater than 0!");
    }
    e_c = tpvs;
  }
  
  if (nrhs > 4){
    double tpvs;
    tpvs = mxGetScalar(prhs[4]);
    if (tpvs < 10 || floor(tpvs)!=tpvs){
      mexErrMsgTxt("T must be an integer that is greater than or equal to 10!");
    }
    T = tpvs;
  }
  
  if (nrhs > 5){
    double tpvs;
    tpvs = mxGetScalar(prhs[5]);
    if (tpvs < 8){
      mexErrMsgTxt("lambda must be greater than or equal to 8!");
    }
    lamb = tpvs;
  }
  
  if (nrhs > 6){
    double tpvs;
    tpvs = mxGetScalar(prhs[6]);
    if (tpvs <= 0){
      mexErrMsgTxt("e_s must be greater than 0!");
    }
    e_s = tpvs;
  }
  
  
  mexPrintf("v_x = %d v_y = %d \n", v_x, v_y);
  //if (nrhs > 3){
    mexPrintf("e_c    = %f\n", e_c);
    mexPrintf("T      = %d\n", T);
    mexPrintf("lambda = %f\n", lamb);
    mexPrintf("e_s    = %f\n", e_s);
  //}
  
  uint8_t *mimg = (uint8_t*)mxGetData(prhs[0]);
  cvtmatp(imgo, mimg, W, H);
  sp::CISP cisp;
  sp::Merge<sp::LabT> mg;
  
  mexCallMATLAB(0, NULL, 0, NULL, "tic");
  cisp.segmentation_S(imgo, v_x, v_y, T, e_c, e_s, lamb);
  int nsp = mg.merge(cisp.oL.data, cisp.oL.steps, cisp.lab.data, cisp.lab.steps, cisp.oL.width, cisp.oL.height, (v_x*v_y) >> 2);
  mexCallMATLAB(0, NULL, 0, NULL, "toc");
  
  sp::Util::relabel(cisp.oL, 1);
  mexPrintf("%d superpixels have been generated.\n\n", nsp); 
  
  plhs[0] = mxCreateDoubleMatrix(H, W, mxREAL);
  
  double *ml = (double*)mxGetData(plhs[0]);
  cvtmatl(ml, cisp.oL, W, H);
  return;
}