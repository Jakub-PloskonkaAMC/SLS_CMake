/*
 * timers.c
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#include "timers.h"
#include "acquisition.h"
#include "debug.h"
#include "rs485.h"
#include "board.h"
#include "comm_app.h"
#include "conf.h"
#include "ad7768.h"
#include "temp.h"

#ifdef  SENSOR_TYPE_HF
#define PRESCALER 25 //99 = 4 kHz, 19 = 20kHz, 15 = 25kHz // poszukiwanie 20k dla 100Mhz
#define PERIOD 199
#elif defined SENSOR_TYPE_LF
#else
#error  Sensor type not defined! Sensor should be HF or LF type
#endif

TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim9;

uint8_t drdy;

// ROUTINE USAGE BELOW
void TimerSampling_Start(void)
{
	HAL_TIM_Base_Start_IT(&htim7);
}

void TimerSampling_Stop(void)
{
	HAL_TIM_Base_Stop_IT(&htim7);
}

void TimerTemperature_Start(void)
{
	HAL_TIM_Base_Start_IT(&htim9);
}

void TimerSleep_Start(void)
{
	__HAL_TIM_CLEAR_FLAG(&htim10, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim10, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE(&htim10);
}

void TimerSleep_SetTime(uint16_t value)
{
	htim10.Instance->ARR = value;
}
// INIT FUNCTIONS
void TIMER_AcqSample_Init(void)
{
#ifdef SENSOR_TYPE_LF
	return;
#elif defined SENSOR_TYPE_HF
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = PRESCALER;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = PERIOD;
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
	{
		Error_Handler();
	}
#endif
}

void TIMER_TempSample_Init(void)
{
	htim9.Instance = TIM9;
	htim9.Init.Prescaler = 999; // 999 = 40Hz
	htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim9.Init.Period = 1999;
	htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
	{
		Error_Handler();
	}
}

void TIMER_Sleep_Init(void)
{
	htim10.Instance = TIM10;
	htim10.Init.Prescaler = 39999;
	htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim10.Init.Period = 10;
	htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
	{
		Error_Handler();
	}
}
// CALLBACKS BELOW
static void TimerSampling_Elapsed_cb(void)
{
#ifndef SENSOR_TYPE_LF
	drdy = 1;
	//ad7768_reset_cs();

#endif
}

static void TimerSleep_Elapsed_cb(void)
{
	RS485_RX_SleepDone();
}

static void TimerTemperature_Elapsed_cb(void)
{
	Temp_SampleMake();
}
// INTERNALS BELOW
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if (htim_base->Instance == TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM7_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(TIM7_IRQn);
	}
	else if (htim_base->Instance == TIM9)
	{
		__HAL_RCC_TIM9_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 4, 0);
		HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
	}
	else if (htim_base->Instance == TIM10)
	{
		__HAL_RCC_TIM10_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	}
	else
	{
		Error_Handler();
	}
}

void TIM1_UP_TIM10_IRQHandler(void)
{
	if (((htim10.Instance->SR & TIM_FLAG_UPDATE) == TIM_FLAG_UPDATE) != RESET)
	{
		if ((((htim10.Instance->DIER & (TIM_IT_UPDATE)) == (TIM_IT_UPDATE)) ? SET : RESET) != RESET)
		{
			htim10.Instance->SR = ~(TIM_IT_UPDATE);
			TimerSleep_Elapsed_cb();
		}
	}
	htim10.Instance->DIER &= ~(TIM_IT_UPDATE);
	__HAL_TIM_DISABLE(&htim10);
}

void TIM7_IRQHandler(void)
{
	if (((htim7.Instance->SR & TIM_FLAG_UPDATE) == TIM_FLAG_UPDATE) != RESET)
	{
		if ((((htim7.Instance->DIER & (TIM_IT_UPDATE)) == (TIM_IT_UPDATE)) ? SET : RESET) != RESET)
		{
			htim7.Instance->SR = ~(TIM_IT_UPDATE);
			TimerSampling_Elapsed_cb();
		}
	}
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
	if (((htim9.Instance->SR & TIM_FLAG_UPDATE) == TIM_FLAG_UPDATE) != RESET)
	{
		if ((((htim9.Instance->DIER & (TIM_IT_UPDATE)) == (TIM_IT_UPDATE)) ? SET : RESET) != RESET)
		{
			htim9.Instance->SR = ~(TIM_IT_UPDATE);
			TimerTemperature_Elapsed_cb();
		}
	}
}

