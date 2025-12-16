/* Copyright 2019-2020 The MathWorks, Inc. */
#include <arm_common_tables.h>
#include <arm_math.h>
#include <arm_const_structs.h>
#include "../Estimates_Lib/rtwtypes.h"

/*
 * CMSIS FFT/IFFT : Complex to complex.
 */
void mw_cmsis_fft_do_complex(creal32_T* y, uint16_T fftLen, uint8_T ifftFlag) {
    
    const static arm_cfft_instance_f32* S;
    switch (fftLen) {
        case 16:
            S = &arm_cfft_sR_f32_len16;
            break;
        case 32:
            S = &arm_cfft_sR_f32_len32;
            break;
        case 64:
            S = &arm_cfft_sR_f32_len64;
            break;
        case 128:
            S = &arm_cfft_sR_f32_len128;
            break;
        case 256:
            S = &arm_cfft_sR_f32_len256;
            break;
        case 512:
            S = &arm_cfft_sR_f32_len512;
            break;
        case 1024:
            S = &arm_cfft_sR_f32_len1024;
            break;
        case 2048:
            S = &arm_cfft_sR_f32_len2048;
            break;
        case 4096:
            S = &arm_cfft_sR_f32_len4096;
            break;
    }
    arm_cfft_f32(S, (float*)&y[0], ifftFlag, 1U);
}
/*
 * CMSIS FFT/IFFT : Real to complex.
 */

void mw_cmsis_fft_do_real(const real32_T* x, creal32_T* y, uint16_T fftLen, uint8_T ifftFlag) {
    static arm_rfft_fast_instance_f32 S;
    // The parameter fftLen specifies the length of RFFT process.
    // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
    arm_rfft_fast_init_f32(&S, fftLen);
    
    arm_rfft_fast_f32(&S, (float*)&x[0], (float*)&y[0], ifftFlag);
    // Post processing for the Real FFT
    int k, q, halfLen;
    halfLen = fftLen / 2;
    y[halfLen].re = ((float)y[0].im);
    y[halfLen].im = (float)0;
    y[0].im = (float)0;
    
    q = halfLen - 1;
    
    for (k = halfLen + 1; k < fftLen; ++k) {
        y[k].re = (float)(y[q].re);
        y[k].im = -((float)(y[q].im));
        q = q - 1;
    }
}

/* LocalWords:  RFFT
 */
