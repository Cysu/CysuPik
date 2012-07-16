#include "mex.h"
#include <limits.h>

void c_getfitpatch(const int n, const int m, const double* img,
        const mxLogical* sourceregion, const mxLogical* markupregion,
        const int r1, const int r2, const int c1, const int c2,
        const double* alpha,
        double* best) {
    
    int i, j, k, x, y;
    int k1, k2;
    int h = r2-r1+1, w = c2-c1+1;
    int coffset = n*m;
    
    double mindif = 1e10;
    double dif, dis;
            
    for (j = 0; j <= m-w; j ++)
        for (i = 0; i <= n-h; i ++) {
            dif = 0;
            for (y = 0; y < w; y ++)
                for (x = 0; x < h; x ++) {
                    // check if this pixel is not source
                    k1 = (j+y)*n + (i+x);
                    if (!sourceregion[k1]) goto invalid;  
                    
                    // check if this pixel needs filled
                    k2 = (c1+y-1)*n + (r1+x-1);
                    if (markupregion[k2]) continue;
                    
                    // calculate dis
//                     dis = (r1-i-1)*(r1-i-1) + (c1-j-1)*(c1-j-1);
//                     alpha = 10*(1.0-exp(-dis));
//                     alpha = 1.0;
                    
                    // calculate dif
                    for (k = 1; k <= 3; k ++) {
                        dif += (img[k1]-img[k2])*(img[k1]-img[k2]) * alpha[j*n+i];
                        if (dif >= mindif) goto invalid;
                        k1 += coffset; k2 += coffset;
                    }
                }
//             mexPrintf("%d %d %d %d %lf\n", i+1,j+1,r1,c1, alpha[j*n+i]);
                       
            mindif = dif;
            best[0] = i+1;
            best[1] = j+1;
            best[2] = i+h+1;
            best[3] = j+w+1;
            
        invalid:
            continue;
        }
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
    int n = (int)mxGetScalar(prhs[0]);
    int m = (int)mxGetScalar(prhs[1]);
    double* img = mxGetPr(prhs[2]);
    mxLogical* sourceregion = mxGetLogicals(prhs[3]);
    mxLogical* markupregion = mxGetLogicals(prhs[4]);
    int r1 = (int)mxGetScalar(prhs[5]);
    int r2 = (int)mxGetScalar(prhs[6]);
    int c1 = (int)mxGetScalar(prhs[7]);
    int c2 = (int)mxGetScalar(prhs[8]);
    double* alpha = mxGetPr(prhs[9]);
    
    double* best;
    plhs[0] = mxCreateDoubleMatrix(4, 1, mxREAL);
    best = mxGetPr(plhs[0]);
	best[0] = best[1] = best[2] = best[3] = 0.0;
    
    c_getfitpatch(n, m, img, sourceregion, markupregion, r1, r2, c1, c2, alpha, best);
}