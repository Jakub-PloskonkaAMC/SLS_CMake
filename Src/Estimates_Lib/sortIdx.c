/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: sortIdx.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 20-Apr-2023 12:35:27
 */

/* Include Files */
#include "../Estimates_Lib/sortIdx.h"

#include "../Estimates_Lib/rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : int idx[7]
 *                float x[7]
 *                int offset
 *                int np
 *                int nq
 *                int iwork[7]
 *                float xwork[7]
 * Return Type  : void
 */
void merge(int idx[7], float x[7], int offset, int np, int nq, int iwork[7],
           float xwork[7])
{
  int j;
  if ((np != 0) && (nq != 0)) {
    int iout;
    int n_tmp;
    int p;
    int q;
    n_tmp = np + nq;
    for (j = 0; j < n_tmp; j++) {
      iout = offset + j;
      iwork[j] = idx[iout];
      xwork[j] = x[iout];
    }
    p = 0;
    q = np;
    iout = offset - 1;
    int exitg1;
    do {
      exitg1 = 0;
      iout++;
      if (xwork[p] <= xwork[q]) {
        idx[iout] = iwork[p];
        x[iout] = xwork[p];
        if (p + 1 < np) {
          p++;
        } else {
          exitg1 = 1;
        }
      } else {
        idx[iout] = iwork[q];
        x[iout] = xwork[q];
        if (q + 1 < n_tmp) {
          q++;
        } else {
          q = iout - p;
          for (j = p + 1; j <= np; j++) {
            iout = q + j;
            idx[iout] = iwork[j - 1];
            x[iout] = xwork[j - 1];
          }
          exitg1 = 1;
        }
      }
    } while (exitg1 == 0);
  }
}

/*
 * File trailer for sortIdx.c
 *
 * [EOF]
 */
