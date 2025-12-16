/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: sortIdx.h
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 20-Apr-2023 12:35:27
 */

#ifndef SORTIDX_H
#define SORTIDX_H

/* Include Files */
#include <stddef.h>
#include <stdlib.h>
#include "../Estimates_Lib/rtwtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void merge(int idx[7], float x[7], int offset, int np, int nq, int iwork[7],
           float xwork[7]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for sortIdx.h
 *
 * [EOF]
 */
