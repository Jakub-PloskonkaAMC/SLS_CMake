/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: minOrMax.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 20-Apr-2023 12:35:27
 */

/* Include Files */
#include "../Estimates_Lib/minOrMax.h"

#include "../Estimates_Lib/rt_nonfinite.h"
#include "../Estimates_Lib/rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const float x[4]
 * Return Type  : float
 */
float minimum(const float x[4])
{
  float ex;
  int idx;
  int k;
  if (!rtIsNaNF(x[0])) {
    idx = 1;
  } else {
    bool exitg1;
    idx = 0;
    k = 2;
    exitg1 = false;
    while ((!exitg1) && (k < 5)) {
      if (!rtIsNaNF(x[k - 1])) {
        idx = k;
        exitg1 = true;
      } else {
        k++;
      }
    }
  }
  if (idx == 0) {
    ex = x[0];
  } else {
    ex = x[idx - 1];
    idx++;
    for (k = idx; k < 5; k++) {
      float f;
      f = x[k - 1];
      if (ex > f) {
        ex = f;
      }
    }
  }
  return ex;
}

/*
 * File trailer for minOrMax.c
 *
 * [EOF]
 */
