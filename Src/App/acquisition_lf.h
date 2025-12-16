/*
 * acquisition_lf.h
 *
 *  Created on: 11.07.2019
 *      Author: mantoniak
 */

#ifndef APP_ACQUISITION_LF_H_
#define APP_ACQUISITION_LF_H_

#include <stdint.h>
#include <stdbool.h>

void ACQ_Init(void);
void ACQ_Start(void);
void ACQ_Stop(void);
uint8_t ACQ_getRandom(void);
bool ACQ_GetByte(uint8_t *byte);
bool ACQ_isOverFlow(void);
void ACQ_clearOverFlow(void);
void ACQ_SampleAppend(uint8_t* sample);
void ACQ_DoJob(void);
uint8_t ACQ_adxl_ST(void);
uint32_t ACQ_GetSamples(uint8_t* data, uint32_t samplesAmmount);
uint32_t ACQ_CheckForAbleSamples(void);

#endif /* APP_ACQUISITION_LF_H_ */
