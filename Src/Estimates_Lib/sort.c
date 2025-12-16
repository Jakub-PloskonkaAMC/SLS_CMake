/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: sort.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 20-Apr-2023 12:35:27
 */

/* Include Files */
#include "../Estimates_Lib/sort.h"

#include "../Estimates_Lib/rt_nonfinite.h"
#include "../Estimates_Lib/rt_nonfinite.h"
#include "../Estimates_Lib/sortIdx.h"

/* Function Definitions */
/*
 * Arguments    : float x[7]
 * Return Type  : void
 */
void sort(float x[7])
{
  float xwork[7];
  float x4[4];
  int idx[7];
  int i1;
  int i2;
  int i3;
  int ib;
  int k;
  int nNaNs;
  signed char idx4[4];
  signed char perm[4];
  x4[0] = 0.0F;
  idx4[0] = 0;
  x4[1] = 0.0F;
  idx4[1] = 0;
  x4[2] = 0.0F;
  idx4[2] = 0;
  x4[3] = 0.0F;
  idx4[3] = 0;
  for (ib = 0; ib < 7; ib++) {
    idx[ib] = 0;
    xwork[ib] = 0.0F;
  }
  nNaNs = 0;
  ib = 0;
  for (k = 0; k < 7; k++) {
    if (rtIsNaNF(x[k])) {
      idx[6 - nNaNs] = k + 1;
      xwork[6 - nNaNs] = x[k];
      nNaNs++;
    } else {
      ib++;
      idx4[ib - 1] = (signed char)(k + 1);
      x4[ib - 1] = x[k];
      if (ib == 4) {
        float f;
        float f1;
        int i4;
        ib = k - nNaNs;
        if (x4[0] <= x4[1]) {
          i1 = 1;
          i2 = 2;
        } else {
          i1 = 2;
          i2 = 1;
        }
        if (x4[2] <= x4[3]) {
          i3 = 3;
          i4 = 4;
        } else {
          i3 = 4;
          i4 = 3;
        }
        f = x4[i1 - 1];
        f1 = x4[i3 - 1];
        if (f <= f1) {
          f = x4[i2 - 1];
          if (f <= f1) {
            perm[0] = (signed char)i1;
            perm[1] = (signed char)i2;
            perm[2] = (signed char)i3;
            perm[3] = (signed char)i4;
          } else if (f <= x4[i4 - 1]) {
            perm[0] = (signed char)i1;
            perm[1] = (signed char)i3;
            perm[2] = (signed char)i2;
            perm[3] = (signed char)i4;
          } else {
            perm[0] = (signed char)i1;
            perm[1] = (signed char)i3;
            perm[2] = (signed char)i4;
            perm[3] = (signed char)i2;
          }
        } else {
          f1 = x4[i4 - 1];
          if (f <= f1) {
            if (x4[i2 - 1] <= f1) {
              perm[0] = (signed char)i3;
              perm[1] = (signed char)i1;
              perm[2] = (signed char)i2;
              perm[3] = (signed char)i4;
            } else {
              perm[0] = (signed char)i3;
              perm[1] = (signed char)i1;
              perm[2] = (signed char)i4;
              perm[3] = (signed char)i2;
            }
          } else {
            perm[0] = (signed char)i3;
            perm[1] = (signed char)i4;
            perm[2] = (signed char)i1;
            perm[3] = (signed char)i2;
          }
        }
        idx[ib - 3] = idx4[perm[0] - 1];
        idx[ib - 2] = idx4[perm[1] - 1];
        idx[ib - 1] = idx4[perm[2] - 1];
        idx[ib] = idx4[perm[3] - 1];
        x[ib - 3] = x4[perm[0] - 1];
        x[ib - 2] = x4[perm[1] - 1];
        x[ib - 1] = x4[perm[2] - 1];
        x[ib] = x4[perm[3] - 1];
        ib = 0;
      }
    }
  }
  if (ib > 0) {
    perm[1] = 0;
    perm[2] = 0;
    perm[3] = 0;
    if (ib == 1) {
      perm[0] = 1;
    } else if (ib == 2) {
      if (x4[0] <= x4[1]) {
        perm[0] = 1;
        perm[1] = 2;
      } else {
        perm[0] = 2;
        perm[1] = 1;
      }
    } else if (x4[0] <= x4[1]) {
      if (x4[1] <= x4[2]) {
        perm[0] = 1;
        perm[1] = 2;
        perm[2] = 3;
      } else if (x4[0] <= x4[2]) {
        perm[0] = 1;
        perm[1] = 3;
        perm[2] = 2;
      } else {
        perm[0] = 3;
        perm[1] = 1;
        perm[2] = 2;
      }
    } else if (x4[0] <= x4[2]) {
      perm[0] = 2;
      perm[1] = 1;
      perm[2] = 3;
    } else if (x4[1] <= x4[2]) {
      perm[0] = 2;
      perm[1] = 3;
      perm[2] = 1;
    } else {
      perm[0] = 3;
      perm[1] = 2;
      perm[2] = 1;
    }
    for (k = 0; k < ib; k++) {
      i2 = perm[k] - 1;
      i1 = ((k - nNaNs) - ib) + 7;
      idx[i1] = idx4[i2];
      x[i1] = x4[i2];
    }
  }
  ib = (nNaNs >> 1) + 7;
  for (k = 0; k <= ib - 8; k++) {
    i2 = (k - nNaNs) + 7;
    i1 = idx[i2];
    idx[i2] = idx[6 - k];
    idx[6 - k] = i1;
    x[i2] = xwork[6 - k];
    x[6 - k] = xwork[i2];
  }
  if ((nNaNs & 1) != 0) {
    ib -= nNaNs;
    x[ib] = xwork[ib];
  }
  if (7 - nNaNs > 1) {
    int iwork[7];
    for (ib = 0; ib < 7; ib++) {
      iwork[ib] = 0;
    }
    i3 = (7 - nNaNs) >> 2;
    i1 = 4;
    while (i3 > 1) {
      if ((i3 & 1) != 0) {
        i3--;
        ib = i1 * i3;
        i2 = 7 - (nNaNs + ib);
        if (i2 > i1) {
          merge(idx, x, ib, i1, i2 - i1, iwork, xwork);
        }
      }
      ib = i1 << 1;
      i3 >>= 1;
      for (k = 0; k < i3; k++) {
        merge(idx, x, k * ib, i1, i1, iwork, xwork);
      }
      i1 = ib;
    }
    if (7 - nNaNs > i1) {
      merge(idx, x, 0, i1, 7 - (nNaNs + i1), iwork, xwork);
    }
  }
}

/*
 * File trailer for sort.c
 *
 * [EOF]
 */
