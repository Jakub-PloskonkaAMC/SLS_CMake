#include "conf.h"

#ifdef SENSOR_TYPE_HF

#include "acquisition.h"
#include "timers.h"
#include "gpio.h"
#include "cyclicBuffer.h"
#include "adxl.h"
#include "board.h"
#include "debug.h"
#include "ad7768.h"
#include "comm_app.h"
#include "rs485.h"
#include "shared_memory.h"

/*! ----------------------------------------------------------------------------
* Variable
------------------------------------------------------------------------------*/
//#define TEST_SAMPLING

extern DataStorage_t dataStorage;

static uint32_t _samplesCounter;
static uint8_t dataResolution;
static float coefficientA;
static float coefficientB;
static float sensitivity;

extern char calculationsInterrupt;
extern float* collectedData;

/*! ----------------------------------------------------------------------------
* Functions
------------------------------------------------------------------------------*/



/******************************************************************************/
uint8_t ACQ_getRandom(void)
{
	//let's roll the dices
	return ad7768_random();
}

/******************************************************************************/
void ACQ_setOverFlow(void)
{
	Sensor.overflow = true;
	dataStorage.general.overFlw = false;
	dataStorage.general.start = 0;
	dataStorage.general.end = 0;
	_samplesCounter = 0;
	Sensor.aqusition=false;
	Sensor.sample_counter = 0;
#ifdef ABLE_TO_SCHEDULE_SENDING
	ResetScheduledSending();
#endif
}
/******************************************************************************/
void ACQ_Init(void)
{
	if (Protocol == MODBUS)
	{
		dataResolution = 4;
//		Sensor.configuration.RS485_speed = speed_115k;
	}
	else
	{
		dataResolution = 3;
		Sensor.configuration.RS485_speed = speed_1500k;
	}
	RS485_Init(Sensor.configuration.RS485_speed);
	AdiConf_t Config;
	Memory_TakeConfig(&Config);
	memcpy(&coefficientA, &Config.X_Calibration_A, sizeof(float));
	memcpy(&coefficientB, &Config.X_Calibration_B, sizeof(float));
	memcpy(&sensitivity, &Config.X_Calibration_Sensitivity, sizeof(float));
	Config.RS485_speed = Sensor.configuration.RS485_speed;
	Memory_SaveConfig(&Config);

	dataStorage.general.overFlw = false;
	dataStorage.general.start = 0;
	dataStorage.general.end = 0;
	_samplesCounter = 0;
	Sensor.sample_counter = 0;

	if (Protocol == MODBUS)
	{
		dataStorage.general.size = dataResolution * DATA_BUFFER_LENGTH_MODBUS;
		memset(dataStorage.modbus.data, 0, BUFFER_SIZE * DATA_BUFFER_LENGTH_MODBUS);
		ACQ_Start();
	}
	else
	{
		dataStorage.general.size = dataResolution * DATA_BUFFER_LENGTH_SLS;
		memset(dataStorage.sls.data, 0, BUFFER_SIZE * DATA_BUFFER_LENGTH_SLS);
	}
}
/******************************************************************************/
void ACQ_DoJob(void)
{
#ifndef TEST_SAMPLING
	static uint8_t sample[BUFFER_SIZE];
	ad7768_get_data(sample);
	ACQ_SampleAppend(sample);
#else
	uint8_t tempData[BUFFER_SIZE];
	memcpy(tempData,&lastSampleX,BUFFER_SIZE);
	ACQ_SampleAppend(tempData);
//	lastSampleX+= 10;
//	if (lastSampleX > 50000)
//	{
//		lastSampleX = 1;
//	}
#endif
}

uint8_t ACQ_adxl_ST(void)
{
	return adxl_GetID();
}



/******************************************************************************/
void ACQ_Start(void)
{
	//CyclicBuffer_flush(&_acqBuff);
	Sensor.overflow = false;
	Sensor.aqusition=true;
	dataStorage.general.overFlw = false;
	dataStorage.general.start = 0;
	dataStorage.general.end = 0;
	_samplesCounter = 0;
	Sensor.sample_counter = 0;
	ad7768_start();
	TimerSampling_Start();
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/******************************************************************************/
void ACQ_Stop(void)
{
	ad7768_stop();
	//dataStorage.general.start = 0;
	//dataStorage.general.end = 0;
	//Sensor.sample_counter = 0;
	//_samplesCounter = 0;
	Sensor.aqusition=false;
	TimerSampling_Stop();
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
}

static volatile uint32_t lastAppend = 4082038;
/******************************************************************************/
void ACQ_SampleAppend(uint8_t* sample)
{
//	uint32_t value = *sample + (*(sample+1) << 8) + (*(sample+2) << 16);
//	uint32_t valToAdd = lastAppend;
//	lastAppend++;
//	if (lastAppend > 4096997)
//	{
//		lastAppend = 4082038;
//	}
//	if (value < 4085000)
//	{
//		lastAppend++;3,
//	}
//	memcpy(dataStorage.sls.data+dataStorage.sls.end,&valToAdd,BUFFER_SIZE);

	int data_buffer_length;
	if (Protocol == MODBUS)
	{
		int32_t intermediateInt = 0;
		memcpy(&intermediateInt, sample, BUFFER_SIZE - 1); // BUFFER SIZE = 3
		float resultingFloat = (float)intermediateInt;

		// Values from document: T1025 ADI QuickStart
		const float range = 5000;
		const float gain = 1;
		const int resolution = 24;
		const float coeff = range / (gain * sensitivity * ((unsigned int)1 << (resolution - 1)));	// 0,000014551915 228366851806640625 for default value of sensitivity (40)

		resultingFloat = coefficientA * resultingFloat + coefficientB;
		resultingFloat = resultingFloat * coeff;

		memcpy(dataStorage.modbus.data + dataStorage.modbus.end, &resultingFloat, dataResolution);  // dataResolution = 4
		data_buffer_length = DATA_BUFFER_LENGTH_MODBUS;
	}
	else
	{
		memcpy(dataStorage.sls.data + dataStorage.sls.end, sample, dataResolution); // dataResolution = 3
		data_buffer_length = DATA_BUFFER_LENGTH_SLS;
	}

	dataStorage.general.end += dataResolution;
	_samplesCounter++;
	Sensor.sample_counter++;
	if (dataStorage.general.end == dataResolution * data_buffer_length)
	{
		dataStorage.general.end = 0;
	}
	if (Sensor.sample_counter == Sensor.number_of_samples)
	{
		PRINTF_APP("ACQ done samples collected: %d\n", Sensor.sample_counter);
		if (Protocol == SLS)
		{
			ACQ_Stop();
		}
	}
	if (dataStorage.general.end == dataStorage.general.start)
	{
		PRINTF_APP("Overflow occur XX\n");
		if (Protocol == MODBUS)
		{
			collectedData = (float*)dataStorage.modbus.data;
			calculationsInterrupt = 1;
		}
		ACQ_setOverFlow();
		// ACQ_Stop();
	}
}
/******************************************************************************/
uint32_t ACQ_CheckForAbleSamples(void)
{
	return _samplesCounter;
}
/******************************************************************************/
uint32_t ACQ_GetSamples(uint8_t* data, uint32_t samplesAmmount)
{
	uint32_t samplesToTake = samplesAmmount;
	PRINTF_TEST("Samples requested: %d\n",samplesToTake);
	PRINTF_TEST("Samples in buffers: %d\n",Sensor.sample_counter);
	if (samplesToTake > _samplesCounter)
	{
		samplesToTake = _samplesCounter;
	}
	_samplesCounter -= samplesToTake;
	samplesToTake = dataResolution * samplesToTake; // teraz juz bytes to take
	PRINTF_TEST("Bytes to take: %d\n",samplesToTake);
	if (dataStorage.general.start + samplesToTake < dataStorage.general.size)
	{
		/*if (dataStorage.general.start + samplesToTake > dataStorage.general.end && Sensor.aqusition)
		{
			PRINTF_APP("Jakas dziwna sytuacja...\n");
			ACQ_setOverFlow();
			ACQ_Stop();
			ACQ_Init();
			return 0;
		}*/
		memcpy(data,dataStorage.sls.data+dataStorage.general.start,samplesToTake);
		PRINTF_TEST("Samples form %d to %d taken\n",dataStorage.general.start/ dataResolution,(dataStorage.general.start+samplesToTake)/ dataResolution);
		PRINTF_TEST("Bytes form %d to %d taken\n",dataStorage.general.start,dataStorage.general.start+samplesToTake);
		dataStorage.general.start += samplesToTake;
		if (dataStorage.general.start == dataResolution *DATA_BUFFER_LENGTH_SLS)
		{
			dataStorage.general.start = 0;
		}
		else if (dataStorage.general.start > dataResolution *DATA_BUFFER_LENGTH_SLS)
		{
			PRINTF_TEST("Jakas dziwna sytuacja...1\n");
			ACQ_setOverFlow();
			ACQ_Stop();
			ACQ_Init();
		}
		return samplesToTake;
	}


	// KOPIOWANIE NA DWA RAZY
	uint32_t firstPart = dataStorage.general.size - dataStorage.general.start;
	uint32_t secondPart = samplesToTake - firstPart;

	//FIRST COPYING
	memcpy(data,dataStorage.sls.data+dataStorage.general.start,firstPart);
	dataStorage.general.start += firstPart;
	if (dataStorage.general.start == dataResolution*DATA_BUFFER_LENGTH_SLS)
	{
		dataStorage.general.start = 0;
	}
	else
	{
		PRINTF_TEST("Jakas dziwna sytuacja...2\n");
		ACQ_setOverFlow();
		ACQ_Stop();
		ACQ_Init();
		return 0;
	}
	//SECOND COPYING
	if (dataStorage.general.start + secondPart > dataStorage.general.end && Sensor.aqusition)
	{
		PRINTF_TEST("Jakas dziwna sytuacja...3\n");
		ACQ_setOverFlow();
		ACQ_Stop();
		ACQ_Init();
		return 0;
	}
	memcpy(data+firstPart,dataStorage.sls.data,secondPart);
	dataStorage.general.start += secondPart;
	PRINTF_TEST("Samples second: %d to %d taken\n",0,secondPart);
	PRINTF_TEST("Samples first: %d to %d taken\n",dataStorage.general.start,dataStorage.general.size);
	return samplesToTake;
}
/******************************************************************************/
void ACQ_clearOverFlow(void)
{
	Sensor.overflow = false;
}

#ifdef TEST_SAMPLING
int lastSampleX = -1000;
uint32_t lastSampleY = 2;
uint32_t lastSampleZ = 3;
#endif
/******************************************************************************/
#endif
