/* Copyright 2019-2020 The MathWorks, Inc. */

#include "../Estimates_Lib/rtwtypes.h"

#ifndef MW_CMSIS_FFT_H
#define MW_CMSIS_FFT_H
#ifdef __cplusplus
extern "C" { /* sbcheck:ok:extern_c needed*/
#endif
    /*
     * CMSIS FFT/IFFT : Complex to complex.
     */
    extern void mw_cmsis_fft_do_complex(creal32_T* y, 
            uint16_T fftLen, 
            uint8_T ifftFlag);
    
    /*
     * CMSIS FFT/IFFT : Real to complex.
     */
    extern void mw_cmsis_fft_do_real(const real32_T* x,
            creal32_T* y,
            uint16_T fftLen,
            uint8_T ifftFlag);
    
#ifdef __cplusplus
}
#endif
#endif
