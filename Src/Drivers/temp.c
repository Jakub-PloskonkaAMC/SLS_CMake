/*
 * temp.c
 *
 *  Created on: 09.09.2019
 *      Author: pmarchut
 */


#include "temp.h"
#include "board.h"

ADC_HandleTypeDef hadc1;

#define TEMP_SENSOR_AVG_SLOPE_MV_PER_CELSIUS 2.5f
#define TEMP_SENSOR_VOLTAGE_MV_AT_25 760.0f
#define ADC_REFERENCE_VOLTAGE_MV 3300.0f
#define ADC_MAX_OUTPUT_VALUE 4095.0f

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_ENABLE();
    HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  }
}

void ADC_Temp_Init(void)
{
	  ADC_ChannelConfTypeDef sConfig = {0};

	  hadc1.Instance = ADC1;
	  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	  hadc1.Init.ScanConvMode = DISABLE;
	  hadc1.Init.ContinuousConvMode = DISABLE;
	  hadc1.Init.DiscontinuousConvMode = DISABLE;
	  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  hadc1.Init.NbrOfConversion = 1;
	  hadc1.Init.DMAContinuousRequests = DISABLE;
	  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	  if (HAL_ADC_Init(&hadc1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void Temp_SampleMake(void)
{
	HAL_ADC_Start_IT(&hadc1);
}

static float tempSum = 0;
static uint32_t tempSumsCount = 0;
static uint32_t readCounts = 10;
static float lastTemp = 0;

void Temp_SampleGet(uint8_t* dest)
{
	if (readCounts++ >= 10)
	{
		lastTemp = tempSum/tempSumsCount;
		tempSum = 0;
		tempSumsCount = 0;
		readCounts = 0;
	}
	memcpy(dest,&lastTemp,4);
}

void ADC_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&hadc1);
}

static volatile uint32_t readedAdcTemp;

uint16_t PomiarADC;
float Temperature;
float Vsense;

const float V25 = 0.76; // [Volts]
const float Avg_slope = 0.0025; //[Volts/degree]
const float SupplyVoltage = 3.3; // [Volts]
const float ADCResolution = 4096.0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	PomiarADC = HAL_ADC_GetValue(&hadc1); // Pobranie zmierzonej wartosci
	Vsense = (SupplyVoltage*PomiarADC)/(ADCResolution-1); // Przeliczenie wartosci zmierzonej na napiecie
	Temperature = ((Vsense-V25)/Avg_slope)+25; // Obliczenie temperatury
	tempSum += Temperature;
	tempSumsCount++;
}




