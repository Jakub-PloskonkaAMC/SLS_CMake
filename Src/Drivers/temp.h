/*
 * temp.h
 *
 *  Created on: 09.09.2019
 *      Author: pmarchut
 */

#ifndef DRIVERS_TEMP_H_
#define DRIVERS_TEMP_H_

#include "conf.h"
#include "stm32f4xx_hal.h"

void ADC_Temp_Init(void);
void Temp_SampleGet(uint8_t* dest);
void Temp_SampleMake(void);

#endif /* DRIVERS_TEMP_H_ */
