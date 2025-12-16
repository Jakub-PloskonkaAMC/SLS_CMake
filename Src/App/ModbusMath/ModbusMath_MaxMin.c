/*
 * ModbusMath_MaxMin.c
 *
 *  Created on: 5 sie 2022
 *      Author: midec
 */

#include "debug.h"
#include "conf.h"

bool ModbusMath_MaxMin_enable = false;
uint32_t ModbusMath_MaxMin_MaxValue = 0;
uint32_t ModbusMath_MaxMin_MinValue = 0;

void ModbusMath_MaxMin_Start()
    {
    ModbusMath_MaxMin_enable = true;
    ModbusMath_MaxMin_MaxValue = 0;
    ModbusMath_MaxMin_MinValue = 0;
    }

void ModbusMath_MaxMin_Stop()
    {
    ModbusMath_MaxMin_enable = false;
    }

bool ModbusMath_MaxMin_EnableRead()
    {
    return ModbusMath_MaxMin_enable;
    }

uint32_t ModbusMath_MaxMin_MaxValueRead()
    {
    return ModbusMath_MaxMin_MaxValue;
    }

uint32_t ModbusMath_MaxMin_MinValueRead()
    {
    return ModbusMath_MaxMin_MinValue;
    }

void ModbusMath_MaxMin_Calculate(uint8_t* bufferData, uint32_t length)
    {
uint32_t meanVal = 0;
    for (int i = 0; i <length; i++)
	{
	if(ModbusMath_MaxMin_MaxValue < bufferData[i])
	    {
	    ModbusMath_MaxMin_MaxValue = bufferData [i];
	    }
	if(ModbusMath_MaxMin_MinValue > bufferData[i])
	    {
	    ModbusMath_MaxMin_MinValue = bufferData [i];
	    }
	meanVal+=bufferData [i];

	}
    meanVal= meanVal/length;
    ModbusMath_MaxMin_MaxValue = meanVal;
    }
