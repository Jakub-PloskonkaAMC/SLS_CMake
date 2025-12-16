/*
 * timers.h
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#ifndef DRIVERS_TIMERS_H_
#define DRIVERS_TIMERS_H_

#include "conf.h"

void TimerSampling_Start(void);
void TimerSampling_Stop(void);
void TimerTemperature_Start(void);
void TimerSleep_Start(void);
void TimerSleep_SetTime(uint16_t value);

void TIMER_Sleep_Init(void);
void TIMER_AcqSample_Init(void);
void TIMER_TempSample_Init(void);

#endif /* DRIVERS_TIMERS_H_ */
