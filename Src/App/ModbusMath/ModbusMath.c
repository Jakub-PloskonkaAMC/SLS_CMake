/*
 * ModbusMath.c
 *
 *  Created on: 5 sie 2022
 *      Author: midec
 */

#include "acquisition.h"
#include "board.h"
#include "debug.h"
#include "conf.h"
#include "comm_app.h"
#include "ModbusMath_MaxMin.h"
#include "ModbusMath.h"


ModbusMathOutput_t  ModbusMathOutput;

bool ModbusMath_ACQStart = false;

void ModbusMath_ACQStart_Write(bool state)
    {
    ModbusMath_ACQStart = state;
    Sensor.number_of_samples = Comm_Modbus_MaxSampleAtOnce();
    }

bool ModbusMath_ACQStart_Read()
    {
    return ModbusMath_ACQStart;
    }

void ModbusMath_main()
    {
    if (ModbusMath_ACQStart == true)
	{
	if (Sensor.aqusition == false)
	    {
	    if (ModbusMath_MaxMin_EnableRead())
		{
		ModbusMath_MaxMin_Calculate(Comm_ModbusMath(), Comm_Modbus_MaxSampleAtOnce());
		ModbusMathOutput.MaxMin_MaxValue = ModbusMath_MaxMin_MaxValueRead();
		ModbusMathOutput.MaxMin_MinValue = ModbusMath_MaxMin_MinValueRead();
		PRINTF_APP("Modbus Math MaxMin  Max:%d, Min:%d\n",ModbusMath_MaxMin_MaxValueRead(),ModbusMath_MaxMin_MinValueRead());
		ACQ_Start();
		}
	    }
	}
    }
